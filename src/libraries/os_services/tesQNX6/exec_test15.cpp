/*
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <assert.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>



int main(int argc, char *argv[])
{
	int fd;
	char answer[100];
	int error;
	
      fprintf(stderr,"Step1...\n");
      fd = name_open("foo/the/rampaging/frog",NULL);
      assert(fd!=0 & fd!=-1);
      //printf("Step1.5\n");
      fprintf(stderr,"Step1.5\n");
      delay(2000);
      int ct = 1;
      while (1)
	{
	  fprintf(stderr,"Step2\n");

	  fprintf(stderr,"Step3\n");
	  ct++;
	  //printf("Writing %d\n", out.Content());
	  //fflush(stdout);
	  fprintf(stderr,"Writing \n");
	  error = MsgSend(fd, "Prueba",6,(void *)answer,100);
	  delay(2000);
	}
  return 0;
}
*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/dispatch.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define ATTACH_POINT "foo/the/rampaging/frog"

/* We specify the header as being at least a pulse */
typedef struct _pulse msg_header_t;

/* Our real data comes after the header */
typedef struct _my_data {
    msg_header_t hdr;
    int data;
} my_data_t;



/*** Client Side of the code ***/
int client() {
    my_data_t msg;
    int fd;

    //if ((fd = name_open(ATTACH_POINT, 0)) == -1) {
    if ((fd = open("/dev/name/local/foo/the/rampaging/frog", 0)) == -1) {
        return EXIT_FAILURE;
    }

    /* We would have pre-defined data to stuff here */
    msg.hdr.type = 0x00;
    msg.hdr.subtype = 0x00;

    /* Do whatever work you wanted with server connection */
    for (msg.data=0; msg.data < 5; msg.data++) {
        printf("Client sending %d \n", msg.data);
        if (MsgSend(fd, &msg, sizeof(msg), NULL, 0) == -1) {
            break;
        }
    }

    /* Close the connection */
    name_close(fd);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    int ret;

   
        printf("Running Client ... \n");
        ret = client();   /* see name_open() for this code */
    
    return ret;
}


