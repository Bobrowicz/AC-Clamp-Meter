/*
 * setup.c
 *
 * Created: 4/21/2018 6:13:09 PM
 *  Author: peter
 */ 

#include <setup.h>

void device_init()
{
	TIMER_0_init();
	ADC_0_init();
	USART_0_init();
	SPI_0_init();
}

void i_o_init()
{
	DDRC |= (1 << TIMER_OUT);
	DDRC |= (1 << INSTRUMENTATION_OUT);
	DDRD |= (1 << DISPLAY_DIGIT_TENTHS);
	DDRD |= (1 << DISPLAY_DIGIT_ONES);
	DDRD |= (1 << DISPLAY_DIGIT_TENS);
	DDRD |= (1 << DISPLAY_CLR);
}