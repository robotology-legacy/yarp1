
#ifndef q_sio_utilH
#define q_sio_utilH




 void ErrorPrinting(int f);
 int ErrorMsgBox(char *msgstr);
 int SimpleMsgBox(char *msgstr);
 int SioOpen(char *name, unsigned int baudrate);
 char SioPutChars(int ComPort, char *stuff, int n);
 int SioGetChars(int ComPort, char *stuff, int n);
 //int SioTest(int ComPort);
 char SioClrInbuf(int ComPort);
 char SioChangeBaud(int ComPort, unsigned int baudrate);
 char SioClose(int ComPort);

#endif





