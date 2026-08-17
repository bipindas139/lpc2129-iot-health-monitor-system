#include"header.h"

volatile u8 intr_recv[512],flag;
volatile u32 i;

void uart1_isr(void) __irq
{
	u8 id;
	id = U1IIR & 0x0E;

	if(id == 0x04 || id == 0x0C)   // RDA or CTI
	{
		while(U1LSR & 0x01)        // Data Ready
		{
			if (i < 511) {
				intr_recv[i++] = U1RBR; // Store into  software buffer
			} 
			else {
				i = 0; // Reset to prevent memory overwrite
				intr_recv[i++] = U1RBR;
			}
		}
	}
	flag=1;
	intr_recv[i]=0;
	VICVectAddr =0;
}

void uart1_intr_init(void)
{
	VICIntSelect = 0;
	VICVectCntl4  = 7 | (1<<5);
	VICVectAddr4  = (u32) uart1_isr;
	VICIntEnable = (1<<7);
	U1IER = 0x01;
	U1FCR = 0x41;

}



u8 wifi_start(void)
{
	uart1_init(9600);
	uart1_intr_init();

	uart0_tx_string("\r\nWifi Started ....\r\n");

	i=0;
	flag=0;
	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); // clearing reciever buffer
	uart1_tx_string("AT\r\n");
	while(flag==0);
	delay_ms(1000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) // substring checking and is not present send return 0
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	while(flag==0);
	delay_ms(10000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"ready")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	uart1_tx_string("ATE0\r\n");
	while(flag==0);
	delay_ms(1000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv));
	uart1_tx_string("AT+GMR\r\n");
	while(flag==0);
	delay_ms(2000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	uart1_tx_string("AT+CWMODE=1\r\n");
	while(flag==0);
	delay_ms(2000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	uart1_tx_string("AT+CWMODE?\r\n");
	while(flag==0);
	delay_ms(2000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	uart1_tx_string("AT+CWDHCP=1,1\r\n");
	while(flag==0);
	delay_ms(2000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	uart1_tx_string("AT+CWJAP=\"YOUR_WIFI_SSID\",\"YOUR_WIFI_PASSWORD\"\r\n");
	while(flag==0);
	delay_ms(20000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	uart1_tx_string("AT+CIFSR\r\n");
	while(flag==0);
	delay_ms(3000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	uart1_tx_string("AT+CIPMUX=0\r\n");
	while(flag==0);
	delay_ms(2000);
	uart0_tx_string((s8*)intr_recv);
	if(strstr((s8 *)intr_recv,"OK")) 
	{
		i=0;
		flag=0;
	}
	else
	{
		return 0;
	}

	uart0_tx_string("\r\nDONE  ....\r\n");
	return 1;
}

/*************************************************************************************************************************/

u8 send_to_Thinkspeak(s32 heart_rate, float spo2 , s32 temp_sensor)
{
	s8 data[200];
	s8 cmd[50];
	u8 timming =0;
	VICIntEnClr |=1<<5;	

	memset((u8 *)intr_recv, 0, sizeof(intr_recv)); 
	i = 0;
	flag = 0;
	uart1_tx_string("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
	while(flag==0);
	while( timming ++ < 10)
	{
		delay_ms(1000);
		uart0_tx_string((s8*)intr_recv);
		if(strstr((s8 *)intr_recv,"OK") || strstr((s8 *)intr_recv,"CONNECT") || strstr((s8 *)intr_recv,"CONNECTED") || strstr((s8 *)intr_recv,"ALREADY")) 
		{
			i=0;
			flag=0;
			timming =0;
			break;
		}
	}
	if(timming)
	{
		VICIntEnable |=1<<5;
		return 0;
	};	

	memset((u8 *)intr_recv,0,sizeof(intr_recv));	
	sprintf(data,"GET /update?api_key=YOUR_API_KEY""&field1=%d""&field2=%d""&field3=%.1f"" HTTP/1.1\r\n""Host: api.thingspeak.com\r\n""Connection: close\r\n\r\n",temp_sensor,heart_rate,spo2);
	sprintf(cmd,"AT+CIPSEND=%d\r\n",strlen(data));
	uart1_tx_string(cmd);
	while(flag==0);
	delay_ms(1000);
	if(strchr((char*)intr_recv,'>')==NULL)
	{
		VICIntEnable |=1<<5;
		return 0;
	}
	i=0;
	flag=0;

	memset((u8 *)intr_recv,0,sizeof(intr_recv));
	uart1_tx_string(data);  // sends HTTP request
	delay_ms(1000);
	while(flag==0);
	delay_ms(2000);
	uart0_tx_string((s8*)intr_recv);

	VICIntEnable |=1<<5;
	return 1;
}
