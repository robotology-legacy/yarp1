/*
  Very ugly, hastily converted code
 */


#include "YARPViaVoiceASR.h"

#define MISSINGVOCABS 1

#ifdef DEBUGMSGS
#define RECOMSGS 1
#endif

//#define DBG
#define DBG if (0) 

#ifndef __cplusplus
#define __cplusplus
#endif

#include "gram.h"
#include "guifns.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <deque>
#include <string>

#include "YARPAll.h"
#include "YARPPort.h"
#include "YARPTime.h"
#include "YARPNetworkTypes.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"

#include "conf/speech_posture.h"

#include <time.h>


#include <smapi.h>
#include <vtbnfc.h>

template <class T>
T *AssertValid(T *p)
{
  assert (p!=NULL);
  return p;
}

YARPViaVoiceASR *p_master = NULL;
#define master (*(AssertValid(p_master)))


int no_bounce = 1;

static int CalcFound = 0;

void *phone_session_id = NULL;
void *workaday_session_id = NULL;

int kismet_speaking = 0;
int kismet_heard = 0;
int hearing = 0;

char grammar_path[1000] = "kismetgram1.fsg";

#define SID0 phone_session_id
#define SID1 workaday_session_id


static char labelString[255] = "   << Click here to turn on the microphone";


#define caddr_t void *


#define CLOSE_TIME 500

//defines
#define SHELL_MIN_WIDTH      600
#define SHELL_MAX_WIDTH      600
#define SHELL_MIN_HEIGHT     200
#define SHELL_MAX_HEIGHT     200

enum
{
  ANNOT_BAD = 0,
  ANNOT_GOOD = 1,
  ANNOT_SAY = 100,
  ANNOT_WEAK_SAY = 101,
};

void CalCtrlUsage(void);





#undef CheckSmRC
#define CheckSmRC(fn)


/*----------------------------------------------------*/
/* A couple of important values that get passed about */
/*----------------------------------------------------*/
typedef struct
{
  void * top_level;
  int x_input_id;
} MyClientData;

/*----------------------------------------------------------*/
/* A couple of callbacks - for the exit and mic buttons     */
/*----------------------------------------------------------*/
//static void CloseUpShop  ( Widget, XtPointer, XtPointer );
//static void ToggleMic    ( Widget, XtPointer, XtPointer );

void ConnectStuff ( MyClientData * );
void TurnMicOff   ( );

static int    mic_state = 0;
static void *fsg = NULL;

static char   buffer   [ 255 ];

YARPOutputPortOf<NetInt32> out_led;
YARPOutputPortOf<SpeechPosture> out_posture;

/*----------------------------------------------------------*/
/* Use whatever default speech user info there is..         */
/*----------------------------------------------------------*/
static char   userid   [ 80 ] = SM_USE_CURRENT;
static char   enrollid [ 80 ] = SM_USE_CURRENT;
static char   taskid   [ 80 ] = SM_USE_CURRENT;

/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
void SetButtonLabel ( int Flag )
{
}

char mimicry_vocab[256] = "";
char medium_vocab[256] = "";

deque<string> shortterm_vocab_list;
deque<string> mediumterm_vocab_list;

deque<string> vocabularies;
deque<string> vocabularies_pending;

static void AddMimicryItem(const char *txt,const char *vname)
{
  int rc;
  //  char *vname = mimicry_vocab;
  assert(vname[0] != '\0');
  SM_MSG reply;
  SM_VOCWORD vocwords[1];
  SM_VOCWORD *vocwordsp[1];

  vocwords[0].flags = 0;
  vocwords[0].spelling = (char *)txt;
  vocwords[0].spelling_size = strlen(vocwords[0].spelling)+1;

  vocwordsp[0] = &vocwords[0];

  rc = SmSesAddToVocab(SID0, (char *)vname,
		    1,
		    vocwordsp,
		    &reply);

  if (rc != SM_RC_OK)
    {
      fprintf(stderr,"Died while trying to completely add spelling (%d)\n",
	      rc);
      exit(1);
    }

  printf ( ">>> Added mimicry item \"%s\" to %s list\n", txt, vname);
}

static void WipeMimicryItem(const char *txt,const char *vname)
{
  int rc;
  //  char *vname = mimicry_vocab;
  assert(vname[0] != '\0');
  SM_MSG reply;
  SM_VOCWORD vocwords[1];
  SM_VOCWORD *vocwordsp[1];

  vocwords[0].flags = 0;
  vocwords[0].spelling = (char *)txt;
  vocwords[0].spelling_size = strlen(vocwords[0].spelling)+1;

  vocwordsp[0] = &vocwords[0];

  rc = SmSesRemoveFromVocab(SID0, (char *)vname,
		    1,
		    vocwordsp,
		    &reply);

  if (rc != SM_RC_OK)
    {
      fprintf(stderr,"Died while trying to remove spelling (%d)\n",
	      rc);
      exit(1);
    }

  printf ( ">>> Removed mimicry item \"%s\"\n", txt);
}

static void WipeMimicryItems(deque<string>& v, const char *vname)
{
  for (deque<string>::iterator i = v.begin(); i!=v.end(); i++)
    {
      WipeMimicryItem(i->c_str(),vname);
    }
}


static int DefineVocab ( const char *vocabulary )
{
  SM_VOCWORD   voc_words [ 5 ];
  SM_VOCWORD * voc_ptrs [ 5 ];
  int          i;
  int          rc;

  /*-------------------------------------------------------------------*/
  /* The SmDefineVocab call expects an array of pointers to SM_VOCWORD */
  /* structures rather than the SM_VOCWORDs themself                   */
  /*-------------------------------------------------------------------*/
  for ( i = 0 ; i < 5; i++ )
  {
    voc_ptrs [ i ] = & ( voc_words [ i ] );
  }

  /*-------------------------------------------------------------------*/
  /* Fill in the fields of each of the words that are going into the   */
  /* 'HelloVocab' vocabulary and then tell the reco engine about it    */
  /*-------------------------------------------------------------------*/
  voc_words [ 0 ].spelling      = "hippo pot";
  voc_words [ 0 ].spelling_size = strlen ( voc_words [ 0 ].spelling );
  voc_words [ 0 ].flags         = 0;

  voc_words [ 1 ].spelling      = "foobar";  /* no pronunciation */
  voc_words [ 1 ].spelling_size = strlen ( voc_words [ 1 ].spelling );
  voc_words [ 1 ].flags         = 0;

  assert(SID0!=NULL);
  //char *vname = "MyVeryFirst";
  //strncpy(mimicry_vocab,vname,100);
  rc = SmSesDefineVocab ( SID0, (char*)vocabulary, 0, voc_ptrs, 
			  SmAsynchronous );

  return ( rc );
}

static void CheckVocab(const char *vocabulary)
{
  string v(vocabulary);
  //if (vocabularies.find(v) == vocabularies.end())
    {
      DefineVocab(vocabulary);
      vocabularies.push_back(v);
    }
}

static void CheckVocab()
{
  for (deque<string>::iterator it=vocabularies_pending.begin();
       it!=vocabularies_pending.end(); it++)
    {
      CheckVocab((*it).c_str());
    }
}

static void AddVocab(char *txt)
{
  int rc;
  SM_MSG reply;
  SM_VOCWORD vocwords[1];
  SM_VOCWORD *vocwordsp[1];

  assert(SID0!=NULL);
  rc = SmSesAddPronunciation(SID0, "voc_t_ax_n__s_m_axr",
			  "T AX N S M AXR",
			  0,
			  1,
			  SM_ADD_PRONUNCIATION_REPLACE |
			  SM_ADD_PRONUNCIATION_PHONETIC |
			  0,
			  &reply);

  rc = SmSesAddPronunciation(SID0, "phn_r_ay_t",
			  "R AY T",
			  0,
			  1,
			  SM_ADD_PRONUNCIATION_REPLACE |
			  SM_ADD_PRONUNCIATION_PHONETIC |
			  0,
			  &reply);

  rc = SmSesAddPronunciation(SID0, "phn_l_ae_f_t",
			  "L AE F T",
			  0,
			  1,
			  SM_ADD_PRONUNCIATION_REPLACE |
			  SM_ADD_PRONUNCIATION_PHONETIC |
			  0,
			  &reply);


  if (rc != SM_RC_OK)
    {
      fprintf(stderr,"Died while trying to add spelling (%d)\n",
	      rc);
      exit(1);
    }

  vocwords[0].flags = 0;
  vocwords[0].spelling = "voc_t_ax_n__s_m_axr";
  vocwords[0].spelling_size = strlen(vocwords[0].spelling)+1;

  vocwordsp[0] = &vocwords[0];

  rc = SmSesAddToVocab(SID0, txt,
		    1,
		    vocwordsp,
		    &reply);

  if (rc != SM_RC_OK)
    {
      fprintf(stderr,"Died while trying to completely add spelling (%d)\n",
	      rc);
      exit(1);
    }
}

static int DoSimpleVocab ( )
{
  SM_VOCWORD   voc_words [ 5 ];
  SM_VOCWORD * voc_ptrs [ 5 ];
  int          i;
  int          rc;

  /*-------------------------------------------------------------------*/
  /* The SmDefineVocab call expects an array of pointers to SM_VOCWORD */
  /* structures rather than the SM_VOCWORDs themself                   */
  /*-------------------------------------------------------------------*/
  for ( i = 0 ; i < 5; i++ )
  {
    voc_ptrs [ i ] = & ( voc_words [ i ] );
  }

  /*-------------------------------------------------------------------*/
  /* Fill in the fields of each of the words that are going into the   */
  /* 'HelloVocab' vocabulary and then tell the reco engine about it    */
  /*-------------------------------------------------------------------*/
  voc_words [ 0 ].spelling      = "hippo pot";
  voc_words [ 0 ].spelling_size = strlen ( voc_words [ 0 ].spelling );
  voc_words [ 0 ].flags         = 0;

  voc_words [ 1 ].spelling      = "foobar";  /* no pronunciation */
  voc_words [ 1 ].spelling_size = strlen ( voc_words [ 1 ].spelling );
  voc_words [ 1 ].flags         = 0;

  assert(SID0!=NULL);
  char vname[] = "MyVeryFirst";
  strncpy(mimicry_vocab,vname,100);
  rc = SmSesDefineVocab ( SID0, vname, 0, voc_ptrs, SmAsynchronous );
  char vname2[] = "MyVerySecond";
  strncpy(medium_vocab,vname2,100);
  rc = SmSesDefineVocab ( SID0, vname2, 0, voc_ptrs, SmAsynchronous );

  sprintf ( buffer, "DoSimpleVocab: SmDefineVocab() rc = %d\n", rc );

  return ( rc );
}



/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
void ConnectStuff ( MyClientData * MyData )
{
  static int first = TRUE;
  int        rc;
  int        smc;
  SmArg      smargs [ 30 ];
  int        smc2;
  SmArg      smargs2 [ 30 ];
  char       * cp;
  SM_MSG reply;
  /*-------------------------------------------------------------------*/
  /* These callbacks handle the various messages that the speech       */
  /* engine might be sending back                                      */
  /*-------------------------------------------------------------------*/
  SmHandler ConnectCB0    ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler ConnectCB1    ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler DisconnectCB  ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler SetCB         ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler MicOnCB0      ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler MicOnCB1      ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler MicOffCB      ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler DefineGrammarCB0(SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler DefineGrammarCB1(SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler DefineVocabCB0( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler DefineVocabCB1( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler EnableVocabCB ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler GetNextWordCB ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler RecoWordCB0    ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler RecoWordCB1   ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler RecoPhraseCB0 ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler RecoPhraseCB1 ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler UtteranceCB   ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler FocusCB ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler EngineStateCB ( SM_MSG reply, caddr_t client, caddr_t call_data );
  SmHandler QueryPhraseAlternativesCB 
                          ( SM_MSG reply, caddr_t client, caddr_t call_data );


  if ( first )
  {
    smc = 0;
    SmSetArg ( smargs [ smc ], SmNapplicationName,      "SID0" );  smc++;
    SmSetArg ( smargs [ smc ], SmNexternalNotifier, GuiNotifier ); smc++;
    //SmSetArg ( smargs [ smc ], SmNexternalNotifier,     myNotifier ); smc++;
    //SmSetArg ( smargs [ smc ], SmNexternalNotifierData, MyData );     smc++;
    SmSetArg ( smargs [ smc ], SmNaudioHost, "" );     smc++;

    smc2 = 0;
    SmSetArg ( smargs2 [ smc2 ], SmNapplicationName,      "SID1" ); smc2++;
    SmSetArg ( smargs [ smc ], SmNexternalNotifier, GuiNotifier ); smc++;
    //SmSetArg ( smargs2 [ smc2 ], SmNexternalNotifier,     myNotifier ); smc2++;
    //SmSetArg ( smargs2 [ smc2 ], SmNexternalNotifierData, MyData );     smc2++;
    SmSetArg ( smargs2 [ smc2 ], SmNaudioHost, "" );     smc2++;

    /*-----------------------------------------------------------------*/
    /* The call to SmOpen initializes any data that's inside of libSm  */
    /*-----------------------------------------------------------------*/
    rc = SmSesOpen ( &SID0, smc, smargs );

    if ( rc != SM_RC_OK )
    {
      printf("Failed to open base session\n");
       CheckSmRC("SmOpen() failed");
       exit(1);
    }

#ifdef PAIRED_SESSION
    rc = SmSesOpen ( &SID1, smc2, smargs2 );
    if ( rc != SM_RC_OK )
    {
      printf("Failed to open auxiliary session\n");
      CheckSmRC("SmOpen() failed");
       exit(1);
    }
#endif


    /*-----------------------------------------------------------------*/
    /* Add the callbacks to catch the messages coming back from the    */
    /* reco engine                                                     */
    /*-----------------------------------------------------------------*/
    SmSesAddCallback (SID0, SmNconnectCallback,             ConnectCB0,       NULL );
    SmSesAddCallback (SID0, SmNdisconnectCallback,          DisconnectCB,    NULL );
    SmSesAddCallback (SID0, SmNsetCallback,                 SetCB,           NULL );
    SmSesAddCallback (SID0, SmNmicOnCallback,               MicOnCB0,         NULL );
    SmSesAddCallback (SID0, SmNmicOffCallback,              MicOffCB,        NULL );
    SmSesAddCallback (SID0, SmNenableVocabCallback,         EnableVocabCB,   NULL );
    SmSesAddCallback (SID0, SmNdefineVocabCallback,         DefineVocabCB0,   NULL );
    SmSesAddCallback (SID0, SmNdefineGrammarCallback,       DefineGrammarCB0, NULL );
    SmSesAddCallback (SID0, SmNrecognizeNextWordCallback,   GetNextWordCB,   NULL );
    SmSesAddCallback (SID0, SmNrecognizedWordCallback,      RecoWordCB0,      NULL );
    SmSesAddCallback (SID0, SmNrecognizedPhraseCallback,    RecoPhraseCB0,    (void *)MyData);
    SmSesAddCallback (SID0, SmNutteranceCompletedCallback,  UtteranceCB,     NULL );
    SmSesAddCallback (SID0, SmNengineStateCallback,         EngineStateCB,   NULL );
    SmSesAddCallback (SID0, SmNqueryPhraseAlternativesCallback,  
		    QueryPhraseAlternativesCB, NULL );
    SmSesAddCallback (SID0, SmNfocusGrantedCallback,       FocusCB, NULL );

#ifdef PAIRED_SESSION
    SmSesAddCallback (SID1, SmNenableVocabCallback,         EnableVocabCB,   NULL );
    SmSesAddCallback (SID1, SmNconnectCallback,             ConnectCB1,       NULL );
    SmSesAddCallback (SID1, SmNmicOnCallback,               MicOnCB1,         NULL );
    SmSesAddCallback (SID1, SmNdefineVocabCallback,         DefineVocabCB1,   NULL );
    SmSesAddCallback (SID1, SmNdefineGrammarCallback,       DefineGrammarCB1, NULL );
    SmSesAddCallback (SID1, SmNrecognizeNextWordCallback,   GetNextWordCB,   NULL );
    SmSesAddCallback (SID1, SmNrecognizedWordCallback,      RecoWordCB1,      NULL );
    SmSesAddCallback (SID1, SmNrecognizedPhraseCallback,    RecoPhraseCB1,    (void *)MyData);
    SmSesAddCallback (SID1, SmNengineStateCallback,         EngineStateCB,   NULL );
#endif

    first = FALSE;
  }

  /*-----------------------------------------------------------------*/
  /* Now connect to the engine (asynchronously, which means that     */
  /* the ConnectCB will get the results)                             */
  /*-----------------------------------------------------------------*/
  smc = 0;
  SmSetArg ( smargs [ smc ], SmNuserId,       userid    );  smc++;
  SmSetArg ( smargs [ smc ], SmNenrollId,     enrollid  );  smc++;
  SmSetArg ( smargs [ smc ], SmNtask,         taskid    );  smc++;
  SmSetArg ( smargs [ smc ], SmNrecognize,    TRUE      );  smc++;
  SmSetArg ( smargs [ smc ], SmNoverrideLock, TRUE      );  smc++;

  smc2 = 0;
  SmSetArg ( smargs2 [ smc2 ], SmNuserId,       userid    );  smc2++;
  SmSetArg ( smargs2 [ smc2 ], SmNenrollId,     enrollid  );  smc2++;
  SmSetArg ( smargs2 [ smc2 ], SmNtask,         taskid    );  smc2++;
  SmSetArg ( smargs2 [ smc2 ], SmNrecognize,    TRUE      );  smc2++;
  SmSetArg ( smargs2 [ smc2 ], SmNoverrideLock, TRUE      );  smc2++;
  SmSetArg ( smargs2 [ smc2 ], SmNnavigator,    TRUE      );  smc2++;

  rc = SmSesConnect ( SID0, smc, smargs, SmAsynchronous );
  assert(rc==SM_RC_OK);
#ifdef PAIRED_SESSION
  rc = SmSesConnect ( SID1, smc2, smargs2, SmAsynchronous );
  assert(rc==SM_RC_OK);
  printf("SmSesConnect called on SID1\n");
#endif
  SmSesSet(SID0, SM_OPTIMIZE_PERFORMANCE,SM_OPTIMIZE_DEFAULT, &reply);
  assert(rc==SM_RC_OK);
  SmSesSet(SID0, SM_NOTIFY_ENGINE_STATE,TRUE, &reply);
  assert(rc==SM_RC_OK);
  SmSesSet(SID0, SM_PHRASE_ALTERNATIVES,2, &reply);
  assert(rc==SM_RC_OK);
  SmSesSet(SID0, SM_REJECTION_THRESHOLD,0, &reply);
  assert(rc==SM_RC_OK);
  rc = SmSesSet(SID0, SM_SAVE_AUDIO,SM_SAVE_AUDIO_DEFAULT, &reply);
  assert(rc==SM_RC_OK);

  // Hook up the second system

  CheckSmRC ("ConnectStuff: SmConnect()");

  DBG printf("Session up\n");

  CheckVocab();
}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/

extern "C"
{
void TurnMicOn ( void )
{
  SM_MSG  reply;
  int     rc;

  /*-------------------------------------------------------------------*/
  /* Send a request to tell the engine to turn the mic on.  The reply  */
  /* comes back to the MicOnCB                                         */
  /*-------------------------------------------------------------------*/
  //  if (CalcFound)
  {
     rc = SmSesMicOn ( SID0, SmAsynchronous );
     DBG printf("return is %d\n", rc);
     CheckSmRC ("TurnMicOn: SmMicOn()");
#ifdef PAIRED_SESSION
     rc = SmSesMicOn ( SID1, SmAsynchronous );
     CheckSmRC ("TurnMicOn: SmMicOn()");
#endif
  }
}
}

/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
void TurnMicOff ( )
{
  SM_MSG  reply;
  int     rc;


  /*-------------------------------------------------------------------*/
  /* Same as above, but turning the mic off...                         */
  /*-------------------------------------------------------------------*/
  rc = SmSesMicOff( SID0, SmAsynchronous );

  CheckSmRC ("TurnMicOff: SmMicOff()");

}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
static int DoSimpleGrammar ( const char *name, void *sid )
{
  int    rc;
  char   grammar [ 512 ];
  char   currdir [ 512 ];
  char * cp;
  SM_MSG reply;
  /*-------------------------------------------------------------------*/
  /* The grammar that we're using should be in the same directory as   */
  /* the program was run from - the reco engine expects a complete     */
  /* path to it (or a path relative to where the engine is run from)   */
  /* so we build it using getcwd()                                     */
  /*-------------------------------------------------------------------*/
  cp = ( char * ) getcwd ( currdir, sizeof ( currdir ) - 1 );

  if (!strchr(name,'/'))
    {
      sprintf ( grammar, "%s/%s", currdir, name );
    }
  else
    {
      sprintf ( grammar, "%s", name );
    }

  /*-------------------------------------------------------------------*/
  /* And now just tell the engine where the grammar is and let it do   */
  /* its thing.  The DefineGrammarCB will get the results back..       */
  /*-------------------------------------------------------------------*/

  DoSimpleVocab();

  rc = SmSesDefineGrammar ( sid, (char*)name, grammar, 0, SmAsynchronous );

#ifdef RECOMSGS  //@ABP
  sprintf ( buffer, "%s: rc = %d", "DoSimpleGrammar: SmDefineGrammar", rc );
  fprintf (stderr, "%s\n", buffer);
#endif

  return ( rc );
}
/*---------------------------------------------------------------------*/
/*            Do Load FST grammar file                                 */
/*---------------------------------------------------------------------*/
static int DoLoadFst( const char *name, void *sid )
{
  char   grammar [ 512 ];
  char   currdir [ 512 ];
  char * cp;

  /*-------------------------------------------------------------------*/
  /* The grammar that we're using should be in the same directory as   */
  /* the program was run from - the reco engine expects a complete     */
  /* path to it (or a path relative to where the engine is run from)   */
  /* so we build it using getcwd()                                     */
  /*-------------------------------------------------------------------*/
  cp = ( char * ) getcwd ( currdir, sizeof ( currdir ) - 1 );

  sprintf ( grammar, "%s/%s.fst", currdir, name );
  VtLoadFSG(grammar, &fsg);
}

static void DoGram( const char *name, void *sid )
{
  DoSimpleGrammar(name,sid);
  // DoLoadFst(name,sid);
}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler ConnectCB0 ( SM_MSG reply, caddr_t client, caddr_t call_data)
{
  int    rc;
  char * cp;
  char *UserName;
  //XmString xmstrUserName;

  CheckSmRC("ConnectCB");
  
  rc = SmSesQueryDefault (SID0,SM_DEFAULT_USERID, &reply);
  CheckSmRC("SmQueryDefault");
  
  rc = SmGetUserId (reply, &UserName); 
  
  //xmstrUserName = XmStringCreateLtoR ( UserName, XmSTRING_DEFAULT_CHARSET );
  //XtVaSetValues ( userLabel, XmNlabelString, xmstrUserName, NULL );
  //XmStringFree (xmstrUserName);
  CheckSmRC("SmGetUserId");

  /*-------------------------------------------------------------------*/
  /* We're "here", so we're connected to the engine, so now we define  */
  /* the grammar that we want the engine to use, and make the mic      */
  /* sensitive (so it can be pressed)                                  */
  /*-------------------------------------------------------------------*/

  DBG printf("SID0 connected\n");

  //DoGram("calexpression",SID0);
#ifdef USE_TEST_GRAMMAR
  DoGram("calexpression",SID0);
#else
  DoGram(grammar_path,SID0);
#endif
  DBG printf("SID0 grammar and vocab up\n");

  return ( SM_RC_OK );
}

SmHandler ConnectCB1 ( SM_MSG reply, caddr_t client, caddr_t call_data)
{
  printf("SID1 connected\n");
  DoGram("calexpression",SID1);
  //DoGram("kismetgram1",SID1);
  printf("SID1 grammar and vocab up\n");
  return ( SM_RC_OK );
}

/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler DisconnectCB  ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  return ( SM_RC_OK );
}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler SetCB ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  return ( SM_RC_OK );
}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler MicOnCB ( SM_MSG reply, caddr_t client, caddr_t call_data, void *sid )
{
  CheckSmRC("MicOnCB");

  /*-------------------------------------------------------------------*/
  /* The mic got turned on, sp change its label accordingly            */
  /*-------------------------------------------------------------------*/
  SetButtonLabel ( 1 );

  mic_state = 1;

  /*-------------------------------------------------------------------*/
  /* VERY IMPORTANT - this tells the recognizer to 'go' (ie. start     */
  /* capturing the audio and processing it)                            */
  /*-------------------------------------------------------------------*/
  SmSesRecognizeNextWord ( sid, SmAsynchronous );

  return ( SM_RC_OK );
}

SmHandler MicOnCB0 ( SM_MSG reply, caddr_t client, caddr_t call_data)
{
  DBG printf("Mic going on for SID0\n");
  return MicOnCB(reply,client,call_data,SID0);
}

SmHandler MicOnCB1 ( SM_MSG reply, caddr_t client, caddr_t call_data)
{
  //printf("Mic going on for SID1\n");
  return MicOnCB(reply,client,call_data,SID1);
}

/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler MicOffCB ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  CheckSmRC("MicOffCB");

  /*-------------------------------------------------------------------*/
  /* Mic's off, just change the button label..                         */
  /*-------------------------------------------------------------------*/
  SetButtonLabel ( 0 );

  //XtVaSetValues (micButton, XmNlabelPixmap, btnPixmapOff, NULL );

  mic_state = 0;

  return ( SM_RC_OK );
}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler EnableVocabCB ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  CheckSmRC("EnableVocabCB");

  return ( SM_RC_OK );
}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler DefineVocabCB ( SM_MSG reply, caddr_t client, caddr_t call_data, void *sid )
{
  char          * vocab;
  int             rc;
  int             i;
  SM_VOCWORD    * missing;
  unsigned long   num_missing;

  CheckSmRC("DefineVocabCB");

  SmGetVocabName (reply, & vocab );

  /*-------------------------------------------------------------------*/
  /* Check to see if any of the words from the vocabulary are missing  */
  /* from the recognizers pool(s)                                      */
  /*-------------------------------------------------------------------*/
  rc = SmGetVocWords ( reply, & num_missing, & missing );

  CheckSmRC ("DefineVocabCB SmGetVocWords()");

#ifdef MISSINGVOCABS
  if (num_missing) printf("Missing vocabulary? %d\n", num_missing);
  for ( i = 0 ; i < ( int ) num_missing; i++ )
  {

    sprintf ( buffer, "DefineVocabCB: word [ %d ] = '%s'\n",
              i, missing [ i ].spelling );
    fprintf (stderr,  buffer );

  }
#endif
  /*-------------------------------------------------------------------*/
  /* Enable the vocabulary (tells the recognizer to listen for words   */
  /* from it)                                                          */
  /*-------------------------------------------------------------------*/

  //  AddVocab(vocab);

  SmSesEnableVocab ( sid, vocab, SmAsynchronous );

  DBG printf("Vocabulary %s is active\n",vocab);


  return ( SM_RC_OK );
}

SmHandler DefineVocabCB0 ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  return DefineVocabCB(reply,client,call_data,SID0);
}
SmHandler DefineVocabCB1 ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  return DefineVocabCB(reply,client,call_data,SID1);
}

/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler DefineGrammarCB ( SM_MSG reply, caddr_t client, caddr_t caller, void *sid )
{
  int             rc;
  int             i;
  char            buffer [ 16384 ];
  char          * vocab_name;
  SM_VOCWORD    * missing;
  unsigned long   num_missing;


  /*--------------------------------------------------------------------*/
  /* One of the reasons a grammar can fail to be defined is if the fsg  */
  /* has words that we don't have pronunciations for.  If this happens, */
  /* the missing words are included in the reply structure.             */
  /*--------------------------------------------------------------------*/
  SmGetRc ( reply, & rc );

  if ( rc == SM_RC_NOT_INVOCAB )
  {
    rc = SmGetVocWords ( reply, & num_missing, & missing );

    CheckSmRC("SmGetVocWords");

    if ( num_missing )
    {
#ifdef MISSINGVOCABS
      if (num_missing) printf("Missing vocabulary? %d\n", num_missing);
      sprintf ( buffer, "Missing %d word(s) from '%s': ",
                num_missing, vocab_name );

      for ( i = 0 ; i < ( int ) num_missing ; i++ )
      {
        strcat ( buffer, missing [ i ].spelling );

        strcat ( buffer, " " );
      }

      fprintf (stderr, buffer );
      fprintf (stderr, "\n" );
      exit(1);
#endif
    }

    return ( -1 );
  }

  /*--------------------------------------------------------------------*/
  /* Get the vocabulary name out of the reply structure so we will know */
  /* what grammar (vocabulary) to enable                                */
  /*--------------------------------------------------------------------*/
  rc = SmGetVocabName ( reply, & vocab_name );

  CheckSmRC("SmGetVocabName");

#ifdef MISSINGVOCABS
  DBG  sprintf ( buffer, "DefineGrammarCB: Defined '%s'\n", vocab_name );
  fprintf (stderr, buffer );
#endif

  
  rc = SmSesEnableVocab ( sid, vocab_name, SmAsynchronous );

  printf("^^^ Using grammar %s\n",vocab_name);

  CheckSmRC("SmGetEnableVocab");

  return ( 0 );
}

SmHandler DefineGrammarCB0 ( SM_MSG reply, caddr_t client, caddr_t caller )
{
  return DefineGrammarCB(reply,client,caller,SID0);
}
SmHandler DefineGrammarCB1 ( SM_MSG reply, caddr_t client, caddr_t caller )
{
  return DefineGrammarCB(reply,client,caller,SID1);
}

/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler GetNextWordCB ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  CheckSmRC("GetNextWordCB");

  /*-------------------------------------------------------------------*/
  /* This gets called whenever SmRecognizeNextWord() is called         */
  /*-------------------------------------------------------------------*/

  return ( SM_RC_OK );
}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler RecoWordCB ( SM_MSG reply, caddr_t client, caddr_t call_data, void *sid )
{
  int             rc;
  int             i;
  unsigned long   num_firm;
  SM_WORD       * firm;

  CheckSmRC("NextWordCB");

  YARPViaVoiceASR::Phrase p;
  master.OnHeardPhrase(p);

  /*-------------------------------------------------------------------*/
  /* SOMETIMES, the reco engine will have no idea what was said, and   */
  /* will call the recognized word callback - an application had best  */
  /* have a RecognizedWord callback setup to tell the engine to go     */
  /* again (SmRecognizeNextWord) if this happens..                     */
  /*-------------------------------------------------------------------*/

  rc = SmGetFirmWords (reply, & num_firm, & firm );

#ifdef RECOMSGS
  for ( i = 0 ; i < ( int ) num_firm; i++ )
  {
    sprintf ( buffer, "RecoWordCB: firm[%d] = '%s' ('%s')\n",
              i, firm [ i ].spelling, firm [ i ].vocab );
    fprintf(stdout, buffer);
  }
#endif

  /*-------------------------------------------------------------------*/
  /* Tell the recognizer to 'go' again.  It stops so that if we wanted */
  /* to, we could change vocabs...                                     */
  /*-------------------------------------------------------------------*/
  rc = SmSesRecognizeNextWord ( SID0, SmAsynchronous );

  kismet_heard = 0;

  return ( SM_RC_OK );
}

SmHandler RecoWordCB0 ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  return RecoWordCB(reply,client,call_data,SID0);
}

SmHandler RecoWordCB1 ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  return RecoWordCB(reply,client,call_data,SID1);
}


/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/

class PhraseData
{
public:
  SM_WORD * firm;
  unsigned long int *start_times, *end_times;
  int score;
  long int any_tag;
};

SmHandler RecoPhraseCB ( SM_MSG reply, caddr_t client, caddr_t caller, void*sid )
{
  static string prev_guess;
  static int have_guess = 0;
  //MyClientData *myData = (MyClientData *) client;
  int       rc;
  SM_WORD * firm;
  long any_tag = -1;
  unsigned  long num_firm;
  int       i;
  char      buffer [ 16384 ];
  char buf2[100];
  char      phrase [ 16384 ];
  static char      phone_buffer [ 16384 ];
  char      *phraseArray[50];
  unsigned long int num_times = 0;
  unsigned long int *start_times = NULL;
  unsigned long int *end_times = NULL;
  char      *translateTxt;
  char      phraseSend[ 16384 ];
  unsigned  long flags;
  int       increment = 10;
  short     x, y;
  short score;
  SM_ANNOTATION * annots;
  unsigned  long num_annots;
  int annot_valence = 0;

  SmGetRc ( reply, & rc );
  phraseSend[0] = '\0';
  /*-------------------------------------------------------------------*/
  /* Get the phrase that was recognized out of the reply structure.    */
  /*-------------------------------------------------------------------*/
  if ( rc != SM_RC_OK )
  {
    CheckSmRC ("RecoPhraseCB: SmGetRc returned not OK");

    rc = SmSesRecognizeNextWord ( sid, SmAsynchronous );

    return ( rc );
  }

  //if (sid==NULL) printf("id is null\n");
  DBG if (sid==SID0) printf("id is SID0\n");
  //if (sid==SID1) printf("id is SID1\n");

  /*------------------------------------------------------------------*/
  /* Get the 'phrase state' - this tells (among other things) whether */
  /* the phrase was accepted or rejected by the engine                */
  /*------------------------------------------------------------------*/
  rc = SmGetPhraseState ( reply, & flags );

  if ( rc != SM_RC_OK )
  {
    CheckSmRC ("RecoPhraseCB: SmGetPhraseState returned not OK ");

    rc = SmSesRecognizeNextWord ( sid, SmAsynchronous );

    return ( rc );
  }

 

  /*------------------------------------------------------------------*/
  /* As with a recognized command, extract the recognized words       */
  /* (the 'phrase') from the reply structure                          */
  /*------------------------------------------------------------------*/
  rc = SmGetFirmWords ( reply, & num_firm, & firm );

  if ( rc != SM_RC_OK )
  {
    CheckSmRC ("RecoPhraseCB: SmGetFirmWords returned not OK");

    rc = SmSesRecognizeNextWord ( sid, SmAsynchronous );

    return ( rc );
  }

  rc = SmGetWordTimes (reply, &num_times, &start_times, &end_times );

  if ( rc != SM_RC_OK )
  {
    CheckSmRC ("RecoPhraseCB: SmGetWordTimes returned not OK");

    rc = SmSesRecognizeNextWord ( sid, SmAsynchronous );

    return ( rc );
  }
  /*
  else
    {
      int i;
      printf("Times: ");
      for (i=0; i<num_times; i++)
	{
	  printf("%lu ",end_times[i] - start_times[i]);
	}
      printf("\n");
      fflush(stdout);
    }
  */

  while (shortterm_vocab_list.size()>10)
    {
      WipeMimicryItem(shortterm_vocab_list.front().c_str(),mimicry_vocab);
      shortterm_vocab_list.pop_front();
    }

  /*------------------------------------------------------------------*/
  /* Check the phrase state to see if it was accepted or not..        */
  /*------------------------------------------------------------------*/
  if ( flags & SM_PHRASE_ACCEPTED)   //   || flags & SM_PHRASE_HALTED)
  {
    rc = SmGetAnnotations (reply, & num_annots, & annots );

    assert (rc == SM_RC_OK);

    static int ignore_suggestions = 0;
    int kismet_annotation = -1;

    //printf("Checking for annotations (%d)\n", num_annots);
    int short_term_extend = 0;
    for (i=0; i<num_annots; i++)
      {
	switch (annots[i].type)
	  {
	  case SM_ANNOTATION_STRING:
	    const char *str;
	    str = annots[i].annodata.string;
	    if (!ignore_suggestions)
	      {
		printf("---> Annotation string for %d is %s\n", i, str);
		if (str[0] == '/')
		  {
		    printf("*** %s\n", str+1);
		    fflush(stdout);
		  }
		else
		  {
		    switch (str[0])
		      {
		      case 'w':
			printf("Wiping...\n");
			WipeMimicryItems(shortterm_vocab_list,mimicry_vocab);
			shortterm_vocab_list.clear();
			break;
		      case 'i':
			printf("I will ignore future backdoor commands\n");
			printf("*** heard [tah_tah]\n");
			fflush(stdout);
			ignore_suggestions = 1;
			break;
		      }
		  }
	      }
	    break;
	  case SM_ANNOTATION_NUMERIC:
	    //printf("---> Annotation numeric for %d is %ld\n", i, 
	    //	   annots[i].annodata.numeric);
	    {
	      switch (annots[i].annodata.numeric)
		{
		case ANNOT_SAY:
		  //printf("---> Should add to mimicry vocab\n");
		  short_term_extend = 1;
		  break;
		case ANNOT_WEAK_SAY:
		  //printf("---> Should add to mimicry vocab\n");
		  break;
		case ANNOT_GOOD:
		  annot_valence++;
		  break;
		case ANNOT_BAD:
		  annot_valence--;
		  break;
		default:
		  // Okay, we've got some unrecognized annotation.
		  // We should just pass this on.
		  kismet_annotation = annots[i].annodata.numeric;
		  break;
		}
	    }
	    break;
	  case SM_ANNOTATION_NONE:
	    //	    printf("---> no annotation for %d\n", i);
	    break;
	  }
      }

#ifdef RECOMSGS
    strcpy ( phrase, "Acc: " );
#endif
    if (num_firm > 50 )
    {
#ifdef RECOMSGS
      strcpy (phrase, "Too big to be translated");
#endif
    }
    else
    {
      phone_buffer[0] = '\0';
      int first_phone = 1;
      string next_guess = "";
      int phone_ct = 0;
      strcat (phraseSend, "       " );
      if (annot_valence>0)
	{
	  strcat(phraseSend,"GOOD ");
	  kismet_annotation = ANNOT_GOOD;
	}
      if (annot_valence<0)
	{
	  strcat(phraseSend,"BAD ");
	  kismet_annotation = ANNOT_BAD;
	}
      if (kismet_annotation!=-1)
	{
	  char abuf[256];
	  sprintf(abuf,"ANNOTATION_%d ",kismet_annotation);
	  strcat(phraseSend,abuf);
	}
      for ( i = 0 ; i < num_firm ; i++ )
      {
	int added = 0;
        phraseArray[i] = firm[i].spelling;
	//        strcat (phraseSend, firm [ i ].spelling );
	if (firm[i].spelling[0] == 'p')
	  {
	    if (firm[i].spelling[1] == 'h')
	      {
		if (firm[i].spelling[2] == 'n')
		  {
		    phone_ct++;
		    added = 1;
		    if (phone_ct<4)
		      {
			strcat (phraseSend, firm [ i ].spelling );
			if (!first_phone) next_guess += " ";
			next_guess += firm[i].spelling;
			// We are dealing with a phoneme
			if (short_term_extend)
			  {
			    if (!first_phone) strcat(phone_buffer," ");
			    strcat(phone_buffer,firm[i].spelling);
			    first_phone = 0;
			    AddMimicryItem(phone_buffer,mimicry_vocab);
			    string s(phone_buffer);
			    shortterm_vocab_list.push_back(s);
			  }
			first_phone = 0;
			if (i<num_times)
			  {
			    sprintf(buf2,"<%lu>", end_times[i] - start_times[i]);
			    strcat ( phraseSend, buf2 );
			  }
		      }
		  }
	      }
	  }
	if (!added)
	  {
	    strcat (phraseSend, firm [ i ].spelling );
	  }
        strcat (phraseSend, " " );
      }
      phraseArray[i] = NULL;
      //VtGetTranslation (fsg, phraseArray, &translateTxt);

      if (annot_valence>0)
	{
	  if (have_guess)
	    {
	      printf("Should add %s to mediumterm now\n",prev_guess.c_str());
	      AddMimicryItem(prev_guess.c_str(),medium_vocab);
	    }
	}

      if (next_guess.length()>0)
	{
	  prev_guess = next_guess;
	  have_guess = 1;
	}

      if (!kismet_heard)
	{
	  DBG printf("@@@ %s\n", phraseSend);
	  fflush(stdout);
	  //	  DisplaySendMessage (displayLabel, phraseSend, child, parent, root, False);
	}
      else
	{
	  printf("*** hearing self\n");
	  fflush(stdout);
	}
      kismet_heard = 0;
      //DisplaySendMessage (displayLabel, translateTxt, child, parent, root, CalcFound);
    }

    // Now lets look at some alternate interpretations
    //SmQueryPhraseAlternatives
    SmGetPhraseScore(reply,&score);
    //printf("Score is %d\n", score);


    /*------------------------------------------------------------------*/
    /* And go through them, making a string that has the complete       */
    /* phrase in it so that it can be displayed                         */
    /*------------------------------------------------------------------*/
    for ( i = 0 ; i < num_firm ; i++ )
      {
	any_tag = firm[i].tag;
	
	strcat ( phrase, firm [ i ].spelling );
	
	strcat ( phrase, " " );
	
      }

    //SmSesQueryPhraseAlternatives(sid, 0,SmAsynchronous);

    PhraseData pd;
    pd.firm = firm;
    pd.start_times = start_times;
    pd.end_times = end_times;
    pd.score = score;
    pd.any_tag = any_tag;

    YARPViaVoiceASR::Phrase p;
    p.valid = 1;
    p.data = (void *) (&pd);
    p.word_count = num_firm;
    master.OnHeardPhrase(p);

  }
  else
  {
#ifdef RECOMSGS
    strcpy ( phrase, "Phrase Word Reco: " );
#endif
  }



#ifdef RECOMSGS
  fprintf (stderr,  phrase );
#endif



#if 0
  no_bounce = 1;
  rc = SmSesMicOff ( sid, &reply );
  //CheckSmRC ("TurnMicOff: SmMicOff()");
  //SmGetRc ( reply, & rc );
  if (rc == SM_RC_OK)
    {
      unsigned long int utt_num = SmGetUtteranceNumber(reply,&utt_num);

      if ( rc != SM_RC_OK )
	{
	  CheckSmRC ("RecoPhraseCB: SmGetUtteranceNumber returned not OK");
	  rc = SmSesRecognizeNextWord ( sid, SmAsynchronous );
	  return ( rc );
	}
      
      //printf (" *** UTTERANCE NUMBER %lu\n", utt_num);

      time_t rawtime;
      struct tm * timeinfo;
      
      time ( &rawtime );
      timeinfo = localtime ( &rawtime );
      //printf ( "Current date and time is: %s\n", asctime (timeinfo) );
      timeinfo->tm_year = 2000 + (timeinfo->tm_year % 100);
 
      short file_class = 0;
      char filename[256];
      static int ct = 0;
      //sprintf(filename,"/usr/yarp/audio/test%06d.wav",ct);
      sprintf ( filename, "/usr/yarp/audio/via/cog%04d%02d%02d%02d%02d%02d.wav", 
	       timeinfo->tm_year,
	       timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour,
	       timeinfo->tm_min, timeinfo->tm_sec);
      ct++;
      printf(">>> Saving audio to %s\n", filename);
      rc = SmSesSetDirectory(SID0, file_class, filename, &reply);
      assert(rc==SM_RC_OK);
      //rc = SmSesPlayUtterance(sid,utt_num,0,0,&reply);
      long tags[] = {1, 2, 3};
      tags[0] = any_tag;
      assert(any_tag!=-1);
      rc = SmSesPlayWords(sid,1,tags,SM_PLAY_WORDS_SAVE_WAVFILE|SM_PLAY_WORDS_WITH_SILENCE,&reply);
      if (rc!=SM_RC_OK)
	{
	  printf("ERROR %d\n", rc);
	  fflush(stdout);
	}
      assert(rc==SM_RC_OK);

      //printf (" *** UTTERANCE NUMBER %lu\n", utt_num);

      //SmDiscardUtterance(utt_num, &reply );
      //SmGetRc ( reply, & rc );
    }

  rc = SmSesMicOn ( sid, &reply );
  //CheckSmRC ("TurnMicOn: SmMicOn()");
#endif


  /*------------------------------------------------------------------*/
  /* Tell the engine to 'go' again                                    */
  /*------------------------------------------------------------------*/
  rc = SmSesRecognizeNextWord (sid,  SmAsynchronous );

  return ( rc );
}

SmHandler RecoPhraseCB0 ( SM_MSG reply, caddr_t client, caddr_t caller )
{
  return RecoPhraseCB(reply, client, caller, SID0);
}
SmHandler RecoPhraseCB1 ( SM_MSG reply, caddr_t client, caddr_t caller )
{
  return RecoPhraseCB(reply, client, caller, SID1);
}


SmHandler QueryPhraseAlternativesCB ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  int rc;
  short  score;
  unsigned long int flags;
  SM_WORD * firm;
  unsigned  long num_firm;
  int i;

  //PFQ

  rc = SmGetFlags(reply,&flags);
  assert(rc==SM_RC_OK);
  if (!(flags&SM_QUERY_PHRASE_ALTERNATIVES_REPLY))
    {
      rc = SmGetFirmWords (reply, & num_firm, & firm );
      if ( rc != SM_RC_OK )
	{
	  CheckSmRC ("SmGetFirmWords returned not OK");
	  return ( SM_RC_OK );
	}
      /* // stop printing alternatives
      printf("Alternative: ");
      for ( i = 0 ; i < num_firm ; i++ )
      {
        printf("%s ", firm[i].spelling);
      }
      printf("\n");
      */
       SmSesQueryPhraseAlternatives(SID0, 0,SmAsynchronous);
    }
  else
    {
      //rc = SmRecognizeNextWord ( SmAsynchronous );
    }
  return ( SM_RC_OK );
}

/*---------------------------------------------------------------------*/
/*                                                                     */
/*---------------------------------------------------------------------*/
SmHandler UtteranceCB ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  /*************
  LogMessage ( "UtteranceCB\n" );
  ************/
  /*-------------------------------------------------------------------*/
  /* The engine has turned the mic off and processed all of the audio  */
  /*-------------------------------------------------------------------*/

  return ( SM_RC_OK );
  /*-------------------------------------------------------------------*/
  /* The engine has turned the mic off and processed all of the audio  */
  /*-------------------------------------------------------------------*/

  return ( SM_RC_OK );
}


SmHandler FocusCB ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  return ( SM_RC_OK );
}


SmHandler EngineStateCB ( SM_MSG reply, caddr_t client, caddr_t call_data )
{
  unsigned long int notice;
  static int heard_speech = 0;

  SmGetEngineState(reply,&notice);

  switch (notice)
    {
    case SM_NOTIFY_SPEECH_START:
      if (!no_bounce)
	{
	  //printf("*** hearing begins\n");
	  master.OnHearingBegins();
	  heard_speech = 0;
	  if (!kismet_speaking) kismet_heard = 0;
	  fflush(stdout);
	}
      break;
    case SM_NOTIFY_SPEECH_STOP:
      if (!no_bounce)
	{
	  //printf("*** hearing ends\n");
	  master.OnHearingEnds();
	  hearing = 0;
	  fflush(stdout);
	}
      no_bounce = 0;
      break; 
    case SM_NOTIFY_RECOGNIZED_SPEECH:
      //printf("*** hearing speech\n");
      master.OnHearingSpeech();
      if (kismet_speaking) kismet_heard = 1;
      else kismet_heard = 0;
      heard_speech = 1;
      fflush(stdout);
      break; 
    case SM_NOTIFY_SPEECH_TOO_HIGH:
      printf("*** hearing too_high\n");
      fflush(stdout);
      break;
    case SM_NOTIFY_SPEECH_TOO_LOW:
      printf("*** hearing too_low\n");
      fflush(stdout);
      break;
    default:
      //printf("*** session state %d\n", notice);
      break;
    }

  return ( SM_RC_OK );
}


void ProcessCommand(const char *cmd)
{
  static int ticks = 0;
  static int state = -1;
  int prev_state = state;
  char buf1[256] = "";
  char buf2[256] = "";
  sscanf(cmd,"%s %s", buf1, buf2);
  if (strcmp(buf1,"speaking")==0)
    {
      if (strcmp(buf2,"begins")==0)
	{
	  state = 1;
	  ticks = 10;
	}
      else if (strcmp(buf2,"ends")==0)
	{
	  state = 0;
	}
      else
	{
	  state = -1;
	}
    }
  if (strcmp(buf1,"time")==0 && strcmp(buf2,"second")==0)
    {
      if (ticks>0)
	{
	  ticks--;
	  if (ticks<=0)
	    {
	      state = -1;
	    }
	}
    }

  if (state != prev_state)
    {
      if (!(state==-1&&prev_state==0))
	{
	  fprintf(stderr,"SPEAKING STATE SET AT %d\n", state);
	  fflush(stdout);
	}
      if (state==1)
	{
	  kismet_speaking = 1;
	  kismet_heard = 0;
	}
      else
	{
	  kismet_speaking = 0;
	}
    }
}



int recognizer_main (int argc, char **argv)
{
  //  StartLEDs();

  GuiCreate ( argc, argv );

  ConnectStuff(NULL);

  GuiMainLoop ( );

  return ( 0 );       

}


void CalCtrlUsage(void)
{
    fprintf(stdout,"  Usage:\n");
    fprintf(stdout,"  CalCtrl \n");
    fprintf(stdout,"     No option : Display GUI \n");
}


void LogMessage ( char * buff )
{

  printf ( "%s\n", buff );
}


void YARPViaVoiceASR::Main(const char *grammar_filename)
{
  char *args[] = { "hello", NULL };
  assert(p_master==NULL);
  p_master = this;
  assert(grammar_filename!=NULL);
  strncpy(grammar_path,grammar_filename,sizeof(grammar_path));
  recognizer_main(1,args);
}


int YARPViaVoiceASR::Phrase::GetDuration(int index)
{
  assert(data!=NULL);
  PhraseData& pd = *((PhraseData*)data);
  assert(index>=0 && index<word_count);
  return pd.end_times[index] - pd.start_times[index];
}

void YARPViaVoiceASR::Phrase::GetText(int index, char *buffer, 
				      int buffer_length)
{
  assert(data!=NULL);
  PhraseData& pd = *((PhraseData*)data);
  assert(index>=0 && index<word_count);
  strncpy(buffer,pd.firm[index].spelling,buffer_length);
}


void YARPViaVoiceASR::Phrase::SaveWave(const char *filename)
{
  char buf[1000];
  char   currdir [ 512 ];
  char * cp;
  cp = ( char * ) getcwd ( currdir, sizeof ( currdir ) - 1 );
  
  if (!strchr(filename,'/'))
    {
      sprintf ( buf, "%s/%s", currdir, filename );
    }
  else
    {
      sprintf ( buf, "%s", filename );
    }


  int       rc;
  SM_MSG reply;
  long int any_tag;
  void * sid = SID0;

  no_bounce = 1;
  rc = SmSesMicOff ( sid, &reply );
  //CheckSmRC ("TurnMicOff: SmMicOff()");
  //SmGetRc ( reply, & rc );
  if (rc == SM_RC_OK && data!=NULL)
    {
      PhraseData& pd = *((PhraseData*)data);

      /*
      unsigned long int utt_num = SmGetUtteranceNumber(reply,&utt_num);

      if ( rc != SM_RC_OK )
	{
	  CheckSmRC ("RecoPhraseCB: SmGetUtteranceNumber returned not OK");
	  rc = SmSesRecognizeNextWord ( sid, SmAsynchronous );
	  return ( rc );
	}
      */

      //printf (" *** UTTERANCE NUMBER %lu\n", utt_num);

      DBG printf(">>> Saving audio to %s\n", buf);
      short file_class = 0;
      rc = SmSesSetDirectory(SID0, file_class, buf, &reply);
      assert(rc==SM_RC_OK);
      //rc = SmSesPlayUtterance(sid,utt_num,0,0,&reply);
      long tags[] = {1, 2, 3};
      tags[0] = pd.any_tag;
      assert(any_tag!=-1);
      rc = SmSesPlayWords(sid,1,tags,SM_PLAY_WORDS_SAVE_WAVFILE|SM_PLAY_WORDS_WITH_SILENCE,&reply);
      if (rc!=SM_RC_OK)
	{
	  printf("ERROR %d\n", rc);
	  fflush(stdout);
	}
      assert(rc==SM_RC_OK);

      //printf (" *** UTTERANCE NUMBER %lu\n", utt_num);

      //SmDiscardUtterance(utt_num, &reply );
      //SmGetRc ( reply, & rc );
    }

  rc = SmSesMicOn ( sid, &reply );
  //CheckSmRC ("TurnMicOn: SmMicOn()");
}

void YARPViaVoiceASR::CreateVocabulary(const char *vocabulary)
{
  //DefineVocab(vocabulary);
  string v(vocabulary);
  vocabularies_pending.push_back(v);
}

void YARPViaVoiceASR::AddPhrase(const char *phrase, const char *vocabulary)
{
  AddMimicryItem(phrase,vocabulary);
}


void YARPViaVoiceASR::RemovePhrase(const char *phrase, const char *vocabulary)
{
  WipeMimicryItem(phrase,vocabulary);
}

