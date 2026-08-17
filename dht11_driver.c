#include "header.h"

u8 RH_Int;
u8 RH_Dec;
u8 Temp_Int;
u8 Temp_Dec;
u8 CheckSum;
u32 timeout;

void DHT11_init(void)
{
	IODIR0 |= (1<<DHT_PIN);
	IOSET0 |= (1<<DHT_PIN);
}

u8 DHT11_Start(void)
{
	/* Configure as Output */
	IODIR0 |= (1<<DHT_PIN);

	/* Pull Low for 18ms */
	IOCLR0 = (1<<DHT_PIN);
	delay_ms(18);

	/* Pull High for 30us */
	IOSET0 = (1<<DHT_PIN);
	delay_us(30);

	/* Configure as Input */
	IODIR0 &= ~(1<<DHT_PIN);

	timeout = 10000;


	while((IOPIN0 & (1<<DHT_PIN)) && timeout--);

	if(timeout == 0) return 0;

	timeout = 10000;

	while((!(IOPIN0 & (1<<DHT_PIN))) && timeout--);

	if(timeout == 0) return 0;

	timeout = 10000;

	while((IOPIN0 & (1<<DHT_PIN)) && timeout--);

	if(timeout == 0) return 0;

	return 1;

}


u8 DHT11_ReadByte(void)
{
	u8 i;
	u8 data = 0;
	for(i=0;i<8;i++)
	{
		/* Wait for LOW to complete */
		timeout = 1000;

		while((!(IOPIN0 & (1<<DHT_PIN))) && timeout--);

		if(timeout == 0)
			return 0xFF;

		delay_us(40);

		if(IOPIN0 & (1<<DHT_PIN))
			data = (data<<1) | 1;
		else
			data = (data<<1);

		timeout = 1000;

		while((IOPIN0 & (1<<DHT_PIN)) && timeout--);

		if(timeout == 0)
			return 0xFF;

	}
	return data;
}




u8 DHT11_ReadTemperature(void)
{
	if(DHT11_Start())
	{
		RH_Int   = DHT11_ReadByte();
		RH_Dec   = DHT11_ReadByte();
		Temp_Int = DHT11_ReadByte();
		Temp_Dec = DHT11_ReadByte();
		CheckSum = DHT11_ReadByte();

		if((RH_Int + RH_Dec + Temp_Int + Temp_Dec) == CheckSum)
		{
			return Temp_Int;
		}

	}

	return 0xFF;          // Error

}


