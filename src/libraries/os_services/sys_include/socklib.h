int sockopen(char *host, int port,int *p_assigned_port);
int sockinfo(int sock, char *info);
int sockclose(int sock);
void socklisten(int sock, void (*handler) (void *));
int senddata(int socket, char *buffer, int length);
int recvdata(int socket, char *buffer, int maxsize);
void sockerror(char *msg);

typedef struct {
	char *recvbuffer;
	char *bufp;
	char *bufe;
	int   size;
} recvbuffer_t;

recvbuffer_t *recvBufferCreate(int size);
void recvBufferDestroy(recvbuffer_t *rb);
int recvstring(int socket, char *buffer, int maxsize, recvbuffer_t *r);
int sendstring(int socket, char *string);
