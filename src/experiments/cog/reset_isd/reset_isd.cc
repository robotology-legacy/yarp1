#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MSG0 "MH1,0\x0D\x0A"
#define MSG1 "MH1,1\x0D\x0A"
#define MSG2 "MH1,2\x0D\x0A"

void main ()
{
    int fd = open ("//3/dev/ser1", O_RDWR);
    if (fd < 0)
        printf ("Cannot open serial port\n");

    printf ("Choose message : ");
    int c;
    scanf ("%d", &c); 
    if (c == 0)
        write (fd, MSG0, 7);
    else
    if (c == 1)
        write (fd, MSG1, 7);
    else
        write (fd, MSG2, 7);

    close(fd);
}

