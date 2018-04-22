/*
 * AC Clamp Meter.c
 *
 * Created: 4/21/2018 1:19:46 PM
 * Author : peter
 * 
 */ 

#include "setup.h"
#include "main.h"

volatile int sample = 0;


int main(void)
{
	cli();
	i_o_init();
	device_init();
	sei();
	
	int num = 0;
	long int sum = 0;
	volatile int samples = 40;
	volatile double rms = 0;
	while (1)
	{
		if (sample == 1)
		{
			PORTD |= (1 << INSTRUMENTATION_OUT);
			sample = 0;
			num = ADC_get_conversion(0);
			sum += (num * num);
			samples -= 1;
			PORTD &= ~(1 << INSTRUMENTATION_OUT);
		}
		
		
		if (samples == 0)
		{
			//PORTD |= (1 << INSTRUMENTATION_OUT);
			cli();
			rms = sqrt(sum/40);
			sum = 0;
			samples = 40;
			sei();
			//PORTD &= ~(1 << INSTRUMENTATION_OUT);
		}
	}
	return 0;
}

ISR(TIMER0_COMPA_vect)
{
	sample = 1;
	PORTD ^= (1 << ADC_CLK_OUT);
}