/*
 * setup.c
 *
 * Created: 4/21/2018 6:13:09 PM
 *  Author: peter
 */ 

#include "setup.h"


void device_init()
{
	timer_0_init();
	ADC_init();
}

void i_o_init()
{
	DDRD |= (1 << ADC_CLK_OUT);
	DDRD |= (1 << INSTRUMENTATION_OUT);
}