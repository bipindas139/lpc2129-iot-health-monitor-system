#include"header.h"

/* MAX30102 */
volatile u8 sample_buf[6];

extern s8 flag_LED;
extern u32 ir_buffer[MY_BUFFER_SIZE], red_buffer[MY_BUFFER_SIZE];
static u32 index=0;

/* WARNING */
void warning (void)
{
	flag_LED = 0;

	IOSET0 = GREEN_LED;
	IOCLR0 = RED_LED;
	IOCLR0 = BUZZER;

	lcd_cmd(0x01);
	lcd_cmd(0x80);
	lcd_str("WARNING");
}


void MAX_init(void)
{
	uart0_tx_string("\r\n IN MAX_init...\r\n");

	/* mode configartion 0x09 */
	i2c_write(0xAE,0x09,0x40);
	do
	{
		uart0_tx_string("\r\n Checking power ready \r\n");
		i2c_read(0xae,0x00);
	}while(i2c_read(0xae,0x09) & 0x40);

	/* FIFO configaration(0X08) */
	//i2c_write(0xAE,0x08,0xB0);   //Sample averaging = 32 samples, FIFO rollover enabled
	i2c_write(0xAE,0x08,0x00);   //Sample averaging = 0 samples, FIFO rollover disabled


	/* spo2 configaration 0x0A */
	i2c_write(0xAE,0x0A,0x27);

	/* LED pulse amplitude ( 0x0C - 0X0D) */
	i2c_write(0xAE,0x0C,0x3F);
	i2c_write(0xAE,0x0D,0x24);

	i2c_write(0xAE,0x09,0x03);

	/* clearing the FIFO_WR_PTR, OVF_COUNTER, and FIFO_RD_PTR */
	i2c_write(0xAE,0x04,0x00);
	i2c_write(0xAE,0x05,0x00);
	i2c_write(0xAE,0x06,0x00);

	/* Interrupt */
	i2c_write(0xAE,0x02,0xE0);
	i2c_write(0xAE,0x03,0x02);

	uart0_tx_string("\r\nMAX_init completed...\r\n");
}


u8 spo2_sample(void)
{
	u32 red_raw, IR_raw ;
	u8 i = 0;

	while(i<32)
	{
		i2c_burst_read(0xAE,0x07);
		red_raw = (sample_buf[0] & 0x03)<<16 | (sample_buf[1]<<8) | sample_buf[2] ;
		IR_raw = (sample_buf[3] & 0x03)<<16 | (sample_buf[4]<<8) | sample_buf[5] ;

		ir_buffer[index]=IR_raw;
		red_buffer[index]=red_raw;

		index++;
		if(index==(MY_BUFFER_SIZE))
		{
			index=0;
			return 1;
		}
		i++;
	}
	return 0;
}



u8 temp(void)
{
	u8 temp_fraction;
	i2c_write(0xAE,0x21,0x01);
	while( i2c_read(0xAE,0x21) &1);
	temp_fraction = i2c_read(0xAE,0x20);
	return ( i2c_read(0xAE,0x1F) + ( temp_fraction & 0x0F) );
}


u8 read_FIFO_WR_PTR(void)
{
	u8 res;
	res = i2c_read(0xAE,0x04);
	return   res; // & 0x0f;

}

u8 read_OVR_COUNTER(void)
{
	u8 res;
	res = i2c_read(0xAE,0x05);
	return   res ; //& 0x0f;

}

u8 read_FIFO_RD_PTR(void)
{
	u8 res;
	res = i2c_read(0xAE,0x06);
	return   res ; //& 0x0f;

}


u32 average(u32 * b)
{
	u32 av=0,i;

	for( i=0;i<MY_BUFFER_SIZE;i++)
	{
		av=av+b[i];
	}
	return av/MY_BUFFER_SIZE;
}
