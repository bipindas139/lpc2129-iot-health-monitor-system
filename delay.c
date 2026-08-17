#include"header.h"

void delay_us(unsigned int ms)
{
	T0PR=15-1;
	T0PC=0;
	T0TC=0;
	T0TCR=2;
	T0TCR=1;
	while(T0TC<ms);
	T0TCR=0;
}

void delay_ms(unsigned int ms)
{
	T0PR=15000-1;
	T0PC=0;
	T0TC=0;
	T0TCR=2;
	T0TCR=1;
	while(T0TC<ms);
	T0TCR=0;
}

void delay_sec(unsigned int sec)
{
	T0PR=15000000-1;
	T0PC=0;
	T0TC=0;
	T0TCR=2;
	T0TCR=1;
	while(T0TC<sec);
	T0TCR=0;
}
