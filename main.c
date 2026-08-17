#include"header.h"

volatile u8 timer_flag;
u32 ir_buffer[MY_BUFFER_SIZE], red_buffer[MY_BUFFER_SIZE];
s8 flag_LED;

int main()
{

	float spo2;
	s32 heart_rate, temp_sensor,count ;
	s8  spo2_invalid_count=0, hr_invalid_count=0;
	s32 hr_valid, spo2_valid, i=0 ;

	IODIR0 = IODIR_SET;

	/* Normal state - active LOW */
	IOSET0 = RED_LED;       // RED OFF
	IOSET0 = BUZZER;        // BUZZER OFF
	IOCLR0 = GREEN_LED;     // GREEN ON

	uart0_init(9600);
	uart1_init(9600);
	uart1_intr_init();
	lcd_init();
	i2c_init();
	MAX_init();
	DHT11_init();
	timer1_intr_init();

	uart0_tx_string("\r\n Started... \r\n "	);

	for(i = 0; i < 10; i++)
	{
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_str("Patient Monit...");
		lcd_cmd(0xC0);
		lcd_str("System...");
		delay_ms(500);
	}

	lcd_cmd(0x01);
	lcd_cmd(0x80);
	lcd_str("Searching WIFI");
	count = 0;

	while((wifi_start())==0)
	{
		if( ++count == 10)
		{
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_str("ERROR : WIFI");
			lcd_cmd(0xC0);
			lcd_str("Not Connected");
			break;
		}
	}

	if(count < 10)
	{
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_str("WIFI CONECTED");
		lcd_cmd(0xC0);
		lcd_str("SUCCESSFUL");
		VICIntEnable |=1<<5;
	}

/*********************************************************************************************************************/
	
	while(1)
	{	

		if(spo2_sample())
		{
			if (ir_buffer[0] < 50000) 
			{
				uart0_tx_string("\r\n Sensor not in use. Please place the finger.");
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_str("No Sensor");
				lcd_cmd(0xC0);
				lcd_str("Place Finger");

			}
			else
			{

				calculate_hr_spo2(ir_buffer, red_buffer, MY_BUFFER_SIZE, &heart_rate, &spo2, &hr_valid, &spo2_valid);						

				/* Heart rate */
				if(hr_valid)
				{
					uart0_tx_string("\r\n HR =  "	);
					uart0_intiger(heart_rate); //printf("HR = %d", heart_rate);
					lcd_cmd(0x01);
					lcd_cmd(0x80);
					lcd_str("HR : ");
					lcd_integer(heart_rate);
					flag_LED |=1;
					hr_invalid_count = 0;
				}
				else
				{
					uart0_tx_string("\r\n Invalid HR \r\n "	); 
					lcd_cmd(0x01);
					lcd_cmd(0xC0);
					lcd_str("INVALID HR");
					uart0_tx_string("\r\nHR = ");
					uart0_intiger(hr_valid);
					hr_invalid_count ++ ;
				}
				/* SPO2 */
				if(spo2_valid)
				{
					uart0_tx_string("\r\n SPO2 =  "	);
					uart0_float(spo2); 
					lcd_cmd(0xC0);
					lcd_str("SPO2 : ");
					lcd_float(spo2);
					flag_LED |=2;
					spo2_invalid_count = 0;

				}
				else
				{
					uart0_tx_string("\r\n Invalid SPO2 \r\n "	); 
					lcd_cmd(0xC0);
					lcd_str("INVALID SPO2");
					spo2_invalid_count ++;
				}	


				if(  hr_invalid_count > 50 && spo2_invalid_count > 50)
				{
					lcd_cmd(0xC0);
					lcd_str("Abnormal !");
					warning();
				}					
				else if( hr_invalid_count > 50 )
				{
					lcd_cmd(0xC0);
					lcd_str("Abnormal Pulse");
					warning();
				}
				else if( spo2_invalid_count > 50 )
				{
					lcd_cmd(0xC0);
					lcd_str("Abnormal SPO2");
					warning();
				}


				if(flag_LED == 3)
				{  
					IOSET0 = RED_LED;
					IOSET0 = BUZZER;
					IOCLR0 = GREEN_LED;
				}

				/* Temperature Sensor */
				temp_sensor=DHT11_ReadTemperature();
				uart0_tx_string("\r\ntemp:");
				uart0_intiger(temp_sensor);
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_str("TEMP : ");
				lcd_integer(temp_sensor);
			}


			/* Sending data to THINKSPEAK */
			if(timer_flag)
			{
				timer_flag=0;
				if((send_to_Thinkspeak(heart_rate,  spo2 ,  temp_sensor)==0))
				{
					uart0_tx_string("\r\n !!!!!! ERROR : Unable to send data to thinkspeak !!!!!!!\r\n "	);
				}
				else
				{
					uart0_tx_string("\r\n success!!!!\r\n");
				}
			}

		}



		delay_ms(100);		

	}

}

