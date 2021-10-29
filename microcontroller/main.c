/*
// Group 2: Raj Patel, Zachary Rouviere, Evan Waxman
// Experiment 8 Part 1
// 11/1/21

// Description:
	This C code has 3 main functions, Initialize SPI, Write to EEPROM, and READ EEPROM.
	The Initialization function "init()" will set up SPI as MOSI and enable SPI so that the ATMEGA is the Master.
	The initialization function will also will configure PORTB and PORTE to set the
	Accelerometer and Bluetooth Chip Select high so that the EEPROM data does not get corrupted. Furthermore PORTD and PORTC7 are configured 
	so that data can be sent to the FPGA. When the "EEPROM_Write" function is enabled it will write an 8-bit Key value to address
	3 of the external EEPROM. The "EEPROM_Read()" function will read 8 bytes of data (the Key) from address 3 of the external EEPROM. The data
	is then set via PORTD to the FPGA. 
*/

//#include <asf.h> // Uncomment for Microchip Studio
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define KEY 0x69 // Key value

#define FPGA_0 PORTD0
#define FPGA_1 PORTD1
#define FPGA_2 PORTD2
#define FPGA_3 PORTD3
#define FPGA_4 PORTD4
#define FPGA_5 PORTD5
#define FPGA_6 PORTD6
#define FPGA_7 PORTD7
#define FPGA_BUS PORTD
#define DDR_SPI DDRB
#define DD_MOSI PB2
#define DD_MISO PB3
#define DD_SCK PB1
#define EEPROM_SS PB0
#define BT_SS PE2
#define EEPROM_HOLD PE6
#define ACC_SS PB4
#define wren PF0

void init();
void SPI_MasterInit(void);
void SPI_MasterTransmit(uint8_t cData);
void EEPROM_Write(uint8_t x);
void SPI_MasterReadInit(void);
uint8_t EEPROM_Read();
uint8_t SPI_SlaveReceive(void);





int main (void)
{
	// Initialize Pins and SPI
	init();
	// Set Wren for FPGA low
	PORTC = 0b00000000;
	
	//EEPROM_Write((uint8_t) KEY); //To Write
	
	
	// Read EEPROM data
	PORTD = EEPROM_Read();		// To Read
	//FPGA_BUS = EEPROM_Read();	
	
	//Activate Read Enable
	PORTC = 0b10000000;
	_delay_ms (250);
	PORTC = 0b00000000;

	 while (1){
		//Ending infinite loop
	}

	
}

void init()
{
		//board_init(); // Uncomment for Microchip Studio

		//Set Input/Output settings for FPGA ports
		DDRC = 0b10000000;
		DDRD = 0xff;
		
		// Set Wren for FPGA low
		PORTC = 0x00;
		
		// Initialize SPI		
		SPI_MasterInit();

	
}

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	//SS are active low so disable ACC and EEPROM by setting them high
	DDR_SPI = (1<<ACC_SS)|(1<<DD_MOSI)|(1<<DD_SCK)|(1<<EEPROM_SS);
	
	/* Enable SPI, Master, set clock rate fck/4 */
	SPCR = (1<<SPE)|(1<<MSTR);
	
	// Setting Bluetooth CS high and setting EEPROM_HOLD high
	DDRE = (1<<BT_SS)|(1<<EEPROM_HOLD);
	PORTE = (1<<BT_SS)|(1<<EEPROM_HOLD);
	
	// Setting Accelerometer and EEPROM CS High
	PORTB = (1<<ACC_SS);

	PORTB |= (1<<EEPROM_SS);

}


void SPI_MasterTransmit(uint8_t cData) // function directly taken from the ATMEGA16U4 Data sheet
{
	/* Start transmission */

	SPDR = cData;

	/* Wait for transmission complete */

	while(!(SPSR & (1<<SPIF)));
}


void EEPROM_Write(uint8_t x){
		
		
		// CS Low
		PORTB &= ~(1<<EEPROM_SS);

		// Latch Wren in EEPROM for Write Sequence
		SPI_MasterTransmit((uint8_t) 0x06);
		
		// CS High
		PORTB |= 1<<EEPROM_SS;
		
		// CS Low
		PORTB &= ~(1<<EEPROM_SS);
		
		//Start Write Sequence
		SPI_MasterTransmit((uint8_t) 0x02);
		//Write to address 3 by sending 0x000003 MSB to LSB
		SPI_MasterTransmit((uint8_t) 0x00);
		SPI_MasterTransmit((uint8_t) 0x00);
		SPI_MasterTransmit((uint8_t) 0x03);
		
		//Place Key in address 3
		SPI_MasterTransmit(x);
		
		// CS High
		PORTB |= 1<<EEPROM_SS;
}

void SPI_MasterReadInit(void)
{
	/* Enable SPI, set clock rate fck/4 */
	SPCR = (0<<SPE)|(0<<MSTR);
	
	/* Set MISO and SCK output, all others input */
	//SS are active low so disable ACC and BT by setting them high
	DDR_SPI = (1<<ACC_SS)|(1<<DD_MISO)|(0<<DD_MOSI)|(1<<DD_SCK)|(1<<EEPROM_SS);
	
	SPCR = (1<<SPE);
	PORTB |= 1<<EEPROM_SS;
}

 uint8_t EEPROM_Read(){
	 
	//CS Low
	PORTB &= ~(1<<EEPROM_SS);
	
	//Enable EEPROM Read
	SPI_MasterTransmit((uint8_t) 0x03);

	//Read from address 3 by sending 0x000003 MSB to LSB
	SPI_MasterTransmit((uint8_t) 0x00);
	SPI_MasterTransmit((uint8_t) 0x00);
	SPI_MasterTransmit((uint8_t) 0x03); // For debugging use 0x05
	
	//Get data from address 3
	uint8_t out = SPI_SlaveReceive();
	
	// CS High
	PORTB |= 1<<EEPROM_SS;
	return out;
	
}

uint8_t SPI_SlaveReceive(void) // function directly taken from the ATMEGA16U4 Data sheet
{
	//Send dummy data to get good data from EEPROM
	SPDR = 0x00;
	
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	
	/* Return Data Register */
	return SPDR;
}
