#include"header.h"

extern volatile u8 timer_flag;

void timer1_isr(void) __irq
{
	T1IR = 1;
	uart0_tx_string("\r\n In isr timer \r\n ");
	timer_flag = 1;

	VICVectAddr=0;
}

void timer1_intr_init(void)
{
	VICIntSelect=0;
	VICVectAddr5 = (unsigned int )timer1_isr;
	VICVectCntl5 =5|(1<<5);
	//VICIntEnable |=1<<5;

	T1MR0=30000;
	T1MCR=3;

	T1PC=T1TC=0;
	T1TCR=2;
	T1PR=15000-1;
	T1TCR=1;
}
