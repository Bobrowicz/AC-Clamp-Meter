/*
 * usart.c
 *
 * Created: 4/22/2018 6:30:31 PM
 *  Author: peter
 */ 



#include <clock_config.h>
#include <USART_0.h>
#include <avr/io.h>
#include <util/atomic.h>


int8_t USART_0_init()
{

	// Module is in UART mode
	
	#define BAUD 115200
	#include <util/setbaud.h>
	
	/* Enable USART0 */
	PRR &= ~(1 << PRUSART0);
	
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	UCSR0A = USE_2X << U2X0 /*  */
			| (0 << MPCM0);  /* Multi-processor Communication Mode: disabled */

	UCSR0B =  (1 << RXCIE0)    /* RX Complete Interrupt Enable: enabled */
			| (0 << UDRIE0)  /* USART Data Register Empty Interrupt Enable: disabled */
			| (1 << RXEN0)   /* Receiver Enable: enabled */
			| (1 << TXEN0)   /* Transmitter Enable: enabled */
			| (0 << UCSZ02); /*  */

	// UCSR0C = (0 << UMSEL01) | (0 << UMSEL00) /*  */
	//		 | (0 << UPM01) | (0 << UPM00) /* Disabled */
	//		 | 0 << USBS0 /* USART Stop Bit Select: disabled */
	//		 | (1 << UCSZ01) | (1 << UCSZ00); /* 8-bit */

	return 0;
}

void USART_0_enable()
{
	UCSR0B |= ((1 << TXEN0) | (1 << RXEN0));
}

void USART_0_enable_rx()
{
	UCSR0B |= (1 << RXEN0);
}

void USART_0_enable_tx()
{
	UCSR0B |= (1 << TXEN0);
}

void USART_0_disable()
{
	UCSR0B &= ~((1 << TXEN0) | (1 << RXEN0));
}

uint8_t USART_0_get_data()
{
	return UDR0;
}

bool USART_0_is_tx_ready()
{
	return (UCSR0A & (1 << UDRE0));
}

bool USART_0_is_rx_ready()
{
	return (UCSR0A & (1 << RXC0));
}

bool USART_0_is_tx_busy()
{
	return (!(UCSR0A & (1 << TXC0)));
}

uint8_t USART_0_read()
{
	while (!(UCSR0A & (1 << RXC0)))
	;
	return UDR0;
}

void USART_0_write(const uint8_t data)
{
	UDR0 = data;
	while (!(UCSR0A & (1 << UDRE0)));
}