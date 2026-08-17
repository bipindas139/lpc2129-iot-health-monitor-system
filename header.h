#ifndef HEADER_H
#define HEADER_H

#include <LPC21xx.H>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef char s8;
typedef int s32;
typedef float f32;



#define MY_BUFFER_SIZE 400
#define RED_LED        (1<<17)
#define BUZZER         (1<<20)
#define GREEN_LED      (1<<21)

#define IODIR_SET ((1 << 17) | (1 << 20) | (1 << 21))

#define WIFI_SSID           YOUR_WIFI_SSID
#define WIFI_PASSWORD       YOUR_WIFI_PASSWORD
#define THINGSPEAK_API_KEY  YOUR_API_KEY

#define DHT_PIN 11V

/* Warning */
void warning(void);

/* MAX30102 / Algorithm */
void MAX_init(void);
u8 spo2_sample(void);
u32 average(u32 *);

void calculate_hr_spo2(u32 *ir_buffer, 
		u32 *red_buffer, 
		s32 buffer_length, 
		s32 *heart_rate, 
		f32 *spo2, 
		s32 *hr_valid, 
		s32 *spo2_valid);


/* DHT11 */										 
void DHT11_init(void);
u8 DHT11_Start(void);
u8 DHT11_ReadByte(void);
u8 DHT11_ReadTemperature(void);											 

/* I2C */
void i2c_init(void);
void i2c_write(u32 , u32  , u8 );
u8 i2c_read(u32 , u32 );
u8 read_FIFO_WR_PTR(void);
u8 read_OVR_COUNTER(void);
u8 read_FIFO_RD_PTR(void);
void i2c_burst_read(u32 , u32 );
u8 i2c_wait_for_si(void);

/* LCD */
void lcd_init(void);
void lcd_data( u8 );
void lcd_cmd( u8 );
void lcd_str(u8 *);
void lcd_integer(s32 );
void lcd_float(f32 );								

/* Delay */
void delay_sec(u32 );
void delay_ms(u32 );
void delay_us(u32 );

/* UART0 */
void uart0_init(u32 );
void uart0_tx(u8 );
void uart0_tx_string(char *);
u8  uart0_rx(void);
void uart0_rx_string(u8 *);
void uart0_rx_buffer_ESPwifi(u8 *);
void uart0_float(f32 );
void uart0_intiger(s32 );
void uart0_hex32(u32);
void uart0_hex(u8 );

/* UART1 */
void uart1_init(u32 );
void uart1_tx(u8 );
void uart1_tx_string(char *);
u8  uart1_rx(void);
void uart1_rx_string(u8 *);
void uart1_adc_12_Bit(u32 );
void uart1_float(f32 );
void uart1_intiger(s32 );
void uart1_intr_init(void);

/* Wifi(ESP8266) */
u8 wifi_start(void);
u8 send_to_Thinkspeak(s32 , f32  , s32 );

/* Timer */
void timer1_intr_init(void);

#endif
