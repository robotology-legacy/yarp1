#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void sigtrap(int signum)
{
  printf("Signal %i caught\n",signum);
  exit(0);
}


int main()
{
  signal(2,&sigtrap);
  puts("Awaiting signal 2");
  while(1) sleep(10);
  return 0;
}
