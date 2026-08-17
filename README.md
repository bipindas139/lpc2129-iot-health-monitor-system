# LPC2129 IoT Health Monitor

Firmware for an LPC2129 ARM7 microcontroller that reads heart rate and SpO2 from a MAX30102 and temperature from a DHT11, shows the readings on an LCD, and pushes them to ThingSpeak over an ESP8266. 

Written in Embedded C with direct register access — no vendor HAL.

## How it works

The MCU reads Red and IR samples from the MAX30102 using I2C and uses them to calculate heart rate and SpO2. The DHT11 sensor is used to read temperature, and results are displayed on a 16×2 LCD. The firmware checks whether the HR and SpO2 results are valid or not. If the invalid count reaches the preset limit, a warning indication is generate. A Timer1 interrupt is used for periodic data transmission to ThingSpeak through the ESP8266 using AT commands over UART1. UART0 is used separately for debugging and status messages.

## Hardware

| Component | Function |
|---|---|
| LPC2129 | Main microcontroller |
| MAX30102 | Heart rate and SpO2 measurement |
| DHT11 | Temperature measurement |
| ESP8266 | Wi-Fi and ThingSpeak communication |
| 16x2 LCD | Local display |
| LEDs and buzzer | Status and warning indication |

## Software and Tools

- Embedded C
- Keil µVision
- Flash Magic

## Project Structure

The project contains the following main source files:

- `main.c` - Main application logic and system control
- `header.h` - Shared macros, register definitions, variables, and function prototypes
- `i2c_driver.c` - I2C driver with timeout handling
- `max30102_driver.c` - MAX30102 configuration and sensor data acquisition
- `spo2_hr_algorithm.c` - Heart-rate and SpO2 calculation
- `dht11_driver.c` - DHT11 interface
- `esp8266_driver.c` - ESP8266 AT-command and ThingSpeak communication
- `lcd_4bit_driver.c` - 16x2 LCD driver
- `uart0_driver.c` - UART0 communication driver
- `uart1_driver.c` - UART1 communication and interrupt handling
- `timer1_interrupt.c` - Timer1 interrupt handling
- `delay.c` - Software delay functions
- `Startup.s` - ARM7 startup code
- `IoT.uvproj` - Keil project file

## I2C driver

The I2C driver checks status codes explicitly rather than assuming the bus behaved (0x08 START sent, 0x18 SLA+W ACKed, 0x28 data ACKed, and so on). It also doesn't sit in an unbounded `while(SI == 0)` loop waiting on the hardware. There's a timeout on the SI flag, and if it trips, the driver forces a STOP condition and releases the bus rather than hanging the rest of the firmware.

| Status Code | Meaning |
|---|---|
| `0x08` | START condition transmitted |
| `0x10` | Repeated START condition transmitted |
| `0x18` | SLA+W transmitted, ACK received |
| `0x28` | Data transmitted, ACK received |
| `0x40` | SLA+R transmitted, ACK received |
| `0x50` | Data received, ACK returned |
| `0x58` | Data received, NACK returned |

## Building

Open `IoT.uvproj` in Keil µVision, select the LPC2129 target, and build. Flash the resulting HEX file to the board using Flash Magic.

## Notes

This project is an educational embedded-systems prototype developed for sensor interfacing, embedded communication, data processing, and IoT experimentation.

## Author

Bipindas K V — B.Tech, Electronics and Communication Engineering

## License

MIT License
