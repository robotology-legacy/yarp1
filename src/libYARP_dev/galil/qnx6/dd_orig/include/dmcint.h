#ifndef INCLUDE_INTERRUPTS
#define INCLUDE_INTERRUPTS

//Q4 #include <sys/irqinfo.h>

extern void InstallInterruptHandler(int iIndex);
extern void RemoveInterruptHandler(int iIndex);
extern LONG BUSGetInterruptStatus(int iIndex, PUSHORT pusStatus);

/*void FAR HandleInterrupt(void FAR* p);

pid_t FAR DMCInterrupt0();
pid_t FAR DMCInterrupt1();
pid_t FAR DMCInterrupt2();
pid_t FAR DMCInterrupt3();
pid_t FAR DMCInterrupt4();
pid_t FAR DMCInterrupt5();
pid_t FAR DMCInterrupt6();
pid_t FAR DMCInterrupt7();
pid_t FAR DMCInterrupt8();
pid_t FAR DMCInterrupt9();
pid_t FAR DMCInterrupt10();
pid_t FAR DMCInterrupt11();
pid_t FAR DMCInterrupt12();
pid_t FAR DMCInterrupt13();
pid_t FAR DMCInterrupt14();
pid_t FAR DMCInterrupt15();
*/
#define MAX_INTERRUPTS 16

typedef struct _INTQUEUE
{
	int		Count;
	USHORT	usInterrupt[MAX_INTERRUPTS + 1];
} INTQUEUE;

#endif /* INCLUDE_INTERRUPTS */
