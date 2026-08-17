#include"header.h"

void lcd_data( u8 data)
{ 
	unsigned int t;

	//higher nibble
	IOCLR1=0xfe<<16;
	t=(data & 0xf0)<<16;
	IOSET1=t;
	IOSET1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;

	//Lower nibble
	IOCLR1=0xfe<<16;
	t=(data & 0x0f)<<20;
	IOSET1=t;
	IOSET1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
}

void lcd_cmd( u8 cmd)
{
	unsigned int t;

	//higher nibble
	IOCLR1=0xfe<<16;
	t=(cmd & 0xf0)<<16;
	IOSET1=t;
	IOCLR1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;

	//Lower nibble
	IOCLR1=0xfe<<16;
	t=(cmd & 0x0f)<<20;
	IOSET1=t;
	IOCLR1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
}

void lcd_init(void)
{
	IODIR1=0xfe<<16;
	PINSEL2|=0x0;
	lcd_cmd(0x02);
	lcd_cmd(0x28);
	lcd_cmd(0x0e);
	lcd_cmd(0x01);

}

void lcd_str(u8 *p)
{
	while(*p)
	{
		lcd_data(*p);
		p++;
	}
}

int str_len(char *p)
{
	int i;
	for(i=0;*p;p++,i++);
	return i;
}

void float1(float f)
{
	int num,dec,i,j=0,a[20],t,s;
	num=(int)f;
	dec=(f-num)*1000000;
	for(i=dec;i;i=i/10,j++);
	for(i=0;j;j--,i++,dec=dec/10)
	{
		a[i]=dec%10 + 48;
	}
	a[i]='.';
	t=++i;
	for(i=num;i;i=i/10,j++){}
	for(i=t,s=j;j;j--,i++,num=num/10)
	{
		a[i]=num%10 + 48;
	}

	for(i=6+s;i>=0;i--)
		lcd_data(a[i]);

}

void lcd_integer(int num)
{
	int i,a[10];
	if(num==0)
		lcd_data('0');
	if(num<0)
	{
		num=-num;
		lcd_data('-');
	}
	for(i=0;num;num=num/10,i++)
	{
		a[i]=num%10 +48;
	}
	for(i=i-1;i>=0;i--)
		lcd_data(a[i]);
}

void lcd_float(float num)
{
	u32 intnum,decinum;
	s32 i=0;
	u8 a[10];
	if(num<0)
	{
		uart0_tx_string("\r\n LCD : Num is -ve\r\n");	
		return;
	}
	if(num==0)
	{
		lcd_data('0');
		return;
	}
	intnum=num;
	decinum=((num-intnum)*10);

	if(intnum==0)
	{
		lcd_data('0');
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
			lcd_data(a[i]+48);
		}
	}
	lcd_data('.');

	if(decinum==0)
	{
		lcd_data('0');
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
			lcd_data(a[i]+48);
		}
	}

}	
