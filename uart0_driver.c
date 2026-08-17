#include"header.h"

void uart0_init(unsigned int baud)
{
	unsigned int pclk,d;
	
	PINSEL0 |=0x05;    // Tx -> p0.0 , Rx -> p0.1
	
	if(VPBDIV==0)
		pclk=15000000;
	else if(VPBDIV==1)
		pclk=60000000;
	else if(VPBDIV==2)
		pclk=30000000;

	d=pclk/(16*baud);
	U0LCR=0x83;
	U0DLL=d&0xff;
	U0DLM=(d>>8)&0xff;
	U0LCR=0x03;
}


void uart0_rx_buffer_ESPwifi(u8 *buf)
{
	int i=0;

	while(1)
	{
		buf[i]=uart0_rx();

		if(i>3)
		{
			if(buf[i]=='\n' &&  buf[i-1]=='\r')
			{
				break;
			}
		}

		i++;
	}

	buf[i]=0;
}



void uart0_tx(u8 data)
{
	U0THR=data;
	while(((U0LSR>>5)&1)==0);
}

u8 uart0_rx(void)
{
	while(((U0LSR>>0)&1)==0);
	return U0RBR;
}

void uart0_tx_string(char *p)
{
	while(*p)
	{
		uart0_tx(*p);
		p++;
	}
}

void uart0_rx_string(u8 * s)
{
	int i=0;
	u8 t ;
	while((t=uart0_rx())!='\r')
	{
		uart0_tx(t);
		s[i]=t;
		i++;
	}
	s[i]=0;
}

void uart0_float(float num)
{
	int intpart;
	int frac;

	if(num < 0)
	{
		uart0_tx('-');
		num = -num;
	}

	intpart = (int)num;
	frac = (int)((num - intpart) * 1000 + 0.5);

	uart0_intiger(intpart);

	uart0_tx('.');

	if(frac < 100) uart0_tx('0');
	if(frac < 10)  uart0_tx('0');

	uart0_intiger(frac);
}

void uart0_intiger(s32 num)
{
	u8 a[12];
	s32 i = 0;

	if(num == 0)
	{
		uart0_tx('0');
		return;
	}

	if(num < 0)
	{
		uart0_tx('-');
		num = -num;
	}

	while(num)
	{
		a[i++] = (num % 10) + '0';
		num /= 10;
	}

	while(i)
	{
		uart0_tx(a[--i]);
	}
}

u32 uart0_rx_string_decimal(void)
{
	s32 i=0;
	u32 d=0;
	u8 t,s[32];
	while((t=uart0_rx())!='\r')
	{
		uart0_tx(t);
		s[i]=t;
		i++;
	}
	s[i]=0;
	i=0;
	while(s[i])
	{
		d=(d*10)+(s[i]-48);
		i++;
	}
	return d;	
}

/* Function for 1 byte */
void uart0_hex(u8 num) 
{
	u8 t;
	t = (num>>4&0x0f);

	if(t<10)
	{
		uart0_tx(t+48);
	}
	else
	{
		uart0_tx(t-10+65);
	}
	t = (num&0x0f);

	if(t<10)
	{
		uart0_tx(t+48);
	}
	else
	{
		uart0_tx(t-10+65);
	}
}


void uart0_hex32(u32 num)
{
	int i;
	u8 nibble;

	for(i=7;i>=0;i--)
	{
		nibble = (num>>(i*4)) & 0x0F;

		if(nibble<10)
			uart0_tx(nibble+'0');
		else
			uart0_tx(nibble-10+'A');
	}
}
