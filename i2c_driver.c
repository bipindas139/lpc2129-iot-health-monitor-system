#include"header.h"

#define SI ((I2CONSET >> 3) & 1)
#define I2C_TIMEOUT 10000
extern volatile u8 sample_buf[6];

void i2c_init(void)
{
	// *** I2C Bit freq, setting ***
	// Bit freq. = FCLK/(I2SCLL + I2SCLH)
	//(I2SCLL + I2SCLH) = 15000000/100000; setting as 100kBps

	PINSEL0 &= ~((3<<4) | (3<<6)); // Clear bits 4, 5, 6, 7
	PINSEL0 |= (1<<4) | (1<<6); //SCL = P0.2 & SDA = P0.3

	I2SCLL = (15000000/100000)/2;
	I2SCLH = (15000000/100000)/2;

	I2CONSET = 1<<6 ;
	I2CONCLR = 1<<2;
}


void i2c_burst_read(u32 slave_addr, u32 memory_addr)
{
	//u8 result;
	s8 i,t=0;
	u32 timeout_counter=0;

	// Generate START condition
	I2CONSET = (1<<5); // STA =1;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x08) // checking for start condition
	{
		uart0_tx_string("\r\n ERROR : START condition \r\n");
		goto exit ; 
	}
	I2CONCLR = (1<<5); // STA =0;


	// Sending SA+W
	I2DAT = slave_addr  ; //SA+W
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x18) // checking for SA+W,ACK
	{
		uart0_tx_string("\r\n ERROR : SA + W failed , I2STAT != 0x18 \r\n");
		goto exit; 
	}


	// Sending memory addr
	I2DAT = memory_addr ;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x28)   //  Data/Memory_addr  has been transmitted expected status
	{
		uart0_tx_string("\r\n ERROR : memory_addr \r\n");
		goto exit ; 
	}


	// Generate repeated START condition      
	I2CONSET = (1<<5); // STA =1;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x10)   // Repeated START condition failed
	{
		uart0_tx_string("\r\n ERROR : Restart condition \r\n");
		goto exit; 
	}
	I2CONCLR = (1<<5); // STA =0;


	// Sending SA+R
	I2DAT = slave_addr | 0x01 ; //SA+R
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x40) // Expected status
	{
		uart0_tx_string("\r\n ERROR : SA + R failed, I2STAT != 0x40 \r\n");
		goto exit ; 
	}

	for(i=0;i<5;i++)
	{

		// Receive data and send ACK 
		I2CONSET=(1<<2); // setting AA=1
		I2CONCLR = (1<<3); // Clear SI flag

		if(i2c_wait_for_si() == 0)
		{       
			return; 
		}

		if(I2STAT != 0x50) // Data byte has been received; ACK has been returned
		{
			uart0_tx_string("\r\n ERROR : ACK \r\n");
			goto exit ; 
		}

		sample_buf[t++]= I2DAT;
	}

	//Receive the last data byte and send NACK
	I2CONCLR = 1<<2;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x58) // Data byte has been received; NOT ACK has been returned
	{
		uart0_tx_string("\r\n ERROR : DATA reception \r\n");
		goto exit ; 
	}

	sample_buf[t++]= I2DAT;

exit:
	I2CONSET = (1<<4);
	I2CONCLR = (1<<3);
	while((I2CONSET>>4&1) == 1)
	{
		timeout_counter++;
		if(timeout_counter > I2C_TIMEOUT) 
		{
			uart0_tx_string("\r\n ERROR:I2C Timeout. Releasing bus. \r\n");
			return; // Timeout failed
		}
	}
}

void i2c_write(u32 slave_addr, u32 memory_addr , u8 data)
{
	u32 timeout_counter=0;

	// Generate START condition
	I2CONSET = (1<<5); // STA =1;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x08) // checking for start condition
	{
		uart0_tx_string("\r\n ERROR : START condition \r\n");
		goto exit ; 
	}
	I2CONCLR = (1<<5); // STA =0;


	// sending SA+W
	I2DAT = slave_addr ; //SA+W
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x18) // checking for SA+W,ACK
	{
		uart0_tx_string("\r\n ERROR : SA + W failed , I2STAT != 0x18 \r\n");
		goto exit ; 
	}


	// Sending memory addr
	I2DAT = memory_addr ;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x28)   //  Data/Memory_addr  has been transmitted expected status
	{
		uart0_tx_string("\r\n ERROR : memory_addr \r\n");
		goto exit ; 
	}


	//Send data
	I2DAT = data ;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return; 
	}

	if(I2STAT != 0x28)   //  Expected status
	{
		uart0_tx_string("\r\n ERROR : Data , I2STAT != 0x28 \r\n");
		goto exit ; 
	}

exit:
	I2CONSET = (1<<4);
	I2CONCLR = (1<<3);
	while((I2CONSET>>4&1) == 1)
	{
		timeout_counter++;
		if(timeout_counter > I2C_TIMEOUT) 
		{
			uart0_tx_string("\r\n ERROR:I2C Timeout. Releasing bus. \r\n");
			return; // Timeout failed
		}
	}
}




u8 i2c_read(u32 slave_addr, u32 memory_addr)
{
	u8 result=0;
	u32 timeout_counter=0;

	// Generate START condition
	I2CONSET = (1<<5); // STA =1;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return 0; 
	}

	if(I2STAT != 0x08) // checking for start condition
	{
		uart0_tx_string("\r\n ERROR : START condition \r\n");
		goto exit ; 
	}
	I2CONCLR = (1<<5); // STA =0;


	// Sending SA+W
	I2DAT = slave_addr  ; //SA+W
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return 0; 
	}

	if(I2STAT != 0x18) // checking for SA+W,ACK
	{
		uart0_tx_string("\r\n ERROR : SA + W failed , I2STAT != 0x18 \r\n");
		goto exit; 
	}


	// Sending memory addr
	I2DAT = memory_addr ;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return 0; 
	}

	if(I2STAT != 0x28)   //  Data/Memory_addr  has been transmitted expected status
	{
		uart0_tx_string("\r\n ERROR : memory_addr \r\n");
		goto exit ; 
	}

	// Generate repeated START condition      
	I2CONSET = (1<<5); // STA =1;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return 0; 
	}

	if(I2STAT != 0x10)   //  Reapeted Start condition failed transmitted
	{
		uart0_tx_string("\r\n ERROR : Restart condition \r\n");
		goto exit; 
	}
	I2CONCLR = (1<<5); // STA =0;


	// Sending SA+R
	I2DAT = slave_addr | 0x01  ; //SA+R
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return 0; 
	}

	if(I2STAT != 0x40) // Expected status
	{
		uart0_tx_string("\r\n ERROR : SA + R failed, I2STAT != 0x40 \r\n");
		goto exit ; 
	}

	// Receiving data
	I2CONCLR = 1<<2;
	I2CONCLR = (1<<3); // Clear SI flag

	if(i2c_wait_for_si() == 0)
	{       
		return 0; 
	}

	if(I2STAT != 0x58) // checking for 0x58 status
	{
		uart0_tx_string("\r\n ERROR : DATA reception \r\n");
		goto exit ; 
	}
	result = I2DAT;
exit:
	I2CONSET = (1<<4);
	I2CONCLR = (1<<3);
	while((I2CONSET>>4&1) == 1)
	{
		timeout_counter++;
		if(timeout_counter > I2C_TIMEOUT) 
		{
			uart0_tx_string("\r\n ERROR:I2C Timeout. Releasing bus. \r\n");
			return 0; // Timeout failed
		}
	}
	return result;
}


u8 i2c_wait_for_si(void)
{
	u32 timeout_counter=0;
	while(SI == 0) 
	{
		timeout_counter++;
		if(timeout_counter > I2C_TIMEOUT) 
		{
			uart0_tx_string("\r\n ERROR: I2C Timeout. Bus Reset. \r\n");

			// STOP condition to release the bus
			I2CONSET = (1<<4);          // STO = 1
			I2CONCLR = (1<<5) | (1<<3); // STA = 0, SI = 0

			return 0; // Timeout failed
		}
	}
	return 1;
}
