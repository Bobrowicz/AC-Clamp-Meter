/*
 * SPI_0.c
 *
 * Created: 4/26/2018 5:12:27 PM
 *  Author: peter
 */ 


#include <SPI_0.h>

void SPI_0_init()
{
	DDRB  = (1 << MOSI)
		  | (1 << SCK)				/* Set MOSI and SCK as output */
		  | (1 << SS);
		  
	PRR &= ~(1 << PRSPI);			/* Enable SPI */

	SPCR = (0 << SPIE)				/* SPI Interrupt Enable: disabled */
		 | (1 << SPE)               /* SPI module enable: enabled */
		 | (0 << DORD)				/* Data order: MSB first */
		 | (1 << MSTR)				/* Master/Slave select: Master */
		 | (0 << CPOL)				/* Clock polarity: disabled */
		 | (0 << CPHA)				/* Clock phase: disabled */
		 | (0 << SPR1) 	
		 | (0 << SPR0);				/* SPI Clock rate selection: fosc/4 */

	SPSR = (0 << SPI2X);			/* SPI double speed: disabled */
}

void SPI_0_enable()
{
	SPCR |= (1 << SPE);
}

void SPI_0_disable()
{
	SPCR &= ~(1 << SPE);
}

void SPI_0_write(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF) ));	/* Wait until transmission complete */
}