#include"header.h"

void uart1_init(unsigned int baud)
{
	unsigned int pclk,d;
	if(VPBDIV==0)
		pclk=15000000;
	else if(VPBDIV==1)
		pclk=60000000;
	else if(VPBDIV==2)
		pclk=30000000;
	PINSEL0 &= ~((3<<16) | (3<<18)); 
	PINSEL0 |=(1<<16) | (1<<18); // Tx -> p0.8 , Rx -> p0.9
	d=pclk/(16*baud);
	U1LCR=0x83;
	U1DLL=d&0xff;
	U1DLM=(d>>8)&0xff;
	U1LCR=0x03;
}

void uart1_rx_buffer_ESPwifi(u8 *buf)
{
	int i=0;

	while(1)
	{
		buf[i]=uart1_rx();

		if(i>3)
		{
			if(buf[i]=='\n' && buf[i-1]=='\r')
			{
				break;
			}
		}

		i++;
	}

	buf[i]=0;
}



void uart1_tx(u8 data)
{
	U1THR=data;
	while(((U1LSR>>5)&1)==0);
}

u8 uart1_rx(void)
{
	while(((U1LSR>>0)&1)==0);
	return U1RBR;
}

void uart1_tx_string(char *p)
{
	while(*p)
	{
		uart1_tx(*p);
		p++;
	}
}

void uart1_rx_string(u8 * s)
{
	int i=0;
	u8 t ;
	while((t=uart1_rx())!='\r')
	{
		uart1_tx(t);
		s[i]=t;
		i++;
	}
	s[i]=0;
}

void uart1_float(float num)
{
	u32 intnum,decinum;
	s32 i=0;
	u8 a[7];
	if(num<0)
	{
		uart1_tx_string("\r\n Num is -ve\r\n");
		return;
	}
	if(num==0)
	{
		uart1_tx('0');
		return;
	}
	intnum=num;
	decinum=((num-intnum)*10);

	if(intnum==0)
	{
		uart1_tx('0');
	}
	else {
		while(intnum)
		{
			a[i]=intnum%10;
			intnum/=10;
			i++;
		}
		while(i>0)
		{
			i--;
			uart1_tx(a[i]+48);
		}
	}
	uart1_tx('.');

	if(decinum==0)
	{
		uart1_tx('0');
		return;
	}
	else {

		while(decinum)
		{
			a[i]=decinum%10;
			decinum/=10;
			i++;
		}

		while(i>0)
		{
			i--;
			uart1_tx(a[i]+48);
		}
	}

}

void uart1_intiger(s32 num)
{
	u32 a[10];
	s32 i=0;
	if(num < 0 || num == 0)
	{
		if( num == 0)
		{
			uart1_tx('0');
			return;
		}

		uart1_tx_string("\r\n Negative number \r\n");
		return;
	}
	while(num)
	{
		a[i]=num%10;
		num/=10;
		i++;
	}
	while(i)
	{
		--i;
		uart1_tx(a[i]+48);
	}
}

u32 uart1_rx_string_decimal(void)
{
	s32 i=0;
	u32 d=0;
	u8 t,s[32];
	while((t=uart1_rx())!='\r')
	{
		uart1_tx(t);
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
void uart1_hex(u8 num) 
{
	u8 t;
	t = (num>>4&0x0f);

	if(t<10)
	{
		uart1_tx(t+48);
	}
	else
	{
		uart1_tx(t-10+65);
	}
	t = (num&0x0f);

	if(t<10)
	{
		uart1_tx(t+48);
	}
	else
	{
		uart1_tx(t-10+65);
	}
}
