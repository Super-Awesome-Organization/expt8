/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define KEY 0x69

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
#define DD_MOSI PORTB2
#define DD_MISO PORTB3
#define DD_SCK PORTB1
#define EEPROM_SS PORTB0
#define BT_SS PORTE2
#define ACC_SS PORTB4


void init();
void SPI_MasterInit(void);
void SPI_MasterTransmit(char cData);
void EEPROM_Write(char x);
char EEPROM_Read();
char SPI_SlaveReceive(void);


//variables to communicate with SRAM.s
unsigned short address = 0;
unsigned char SRAMVal = 0;
unsigned short SRAMValAddr = &SRAMVal;


int main (void)
{
	init();
	//Deactivate read enable
	PORTC = 0b00000000;
	
	EEPROM_Write((char) KEY);
	
	
	FPGA_BUS = EEPROM_Read();
	
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
		board_init();

		//Set Input/Output settings for ports
		DDRC = 0b10000000;
		DDRD = 0xff;
		
		//Set read_key = 0;
		PORTC = 0x0;
				
		SPI_MasterInit();

	
}

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	//SS are active low so disable ACC and BT by setting them high
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<ACC_SS);
	DDRE = (1<<BT_SS);

	/* Enable SPI, Master, set clock rate fck/16 */

	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}
void SPI_MasterTransmit(char cData)
{
	/* Start transmission */

	SPDR = cData;

	/* Wait for transmission complete */

	while(!(SPSR & (1<<SPIF)))
	;
}
void EEPROM_Write(char x){
		//Enable EEPROM write
		PORTB = !(1<<EEPROM_SS);
		SPI_MasterTransmit((char) 0x06);
		PORTB = 1<<EEPROM_SS;
		PORTB = !(1<<EEPROM_SS);
		//Start Write Sequence
		SPI_MasterTransmit((char) 0x02);
		//Write to address 0 by sending 0 for 24 bytes
		SPI_MasterTransmit((char) 0x00);
		SPI_MasterTransmit((char) 0x00);
		SPI_MasterTransmit((char) 0x00);
		//Place Data in address 0
		SPI_MasterTransmit(x);
		PORTB = 1<<EEPROM_SS;
	
}
 char EEPROM_Read(){
	//Enable EEPROM write
	PORTB = !(1<<EEPROM_SS);
	SPI_MasterTransmit((char) 0x03);

	//Read from address 0 by sending 0 for 24 bytes
	SPI_MasterTransmit((char) 0x00);
	SPI_MasterTransmit((char) 0x00);
	SPI_MasterTransmit((char) 0x00);
	//Place Data in address 0
	char out = SPI_SlaveReceive();
	PORTB = 1<<EEPROM_SS;
	return out;
	
}
char SPI_SlaveReceive(void)
{
	/* Wait for reception complete /
	while(!(SPSR & (1<<SPIF)))
	;
	/ Return Data Register */
	return SPDR;
}


