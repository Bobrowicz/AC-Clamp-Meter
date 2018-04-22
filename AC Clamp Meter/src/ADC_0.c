/*
 * ADC_0.c
 *
 * Created: 4/21/2018 2:07:20 PM
 *  Author: peter
 */ 

#include "ADC_0.h"

void ADC_init()
{
	ADMUX  = (1 << REFS0)			// AVcc selected
		   | (0 << ADLAR)			// Left Adjust Result disabled
		   | (0 << MUX0);			// ADC Single Ended Input pin 0
		   
	ADCSRA  = (1 << ADEN)			// Enable ADC
	        | (1 << ADPS2) 
	        | (1 << ADPS1) 
	        | (1 << ADPS0);		    // Prescaler set to clk/128
	ADCSRA |= (1 << ADATE);		
	ADCSRB |= (1 << ADTS1)
			| (1 << ADTS0);
}

void ADC_enable()
{
	ADCSRA  |= (1 << ADEN);			// Enable ADC
}

void ADC_disable()
{
	ADCSRA  &= ~(1 << ADEN);		// Disable ADC
}

void ADC_select_chanel(uint8_t channel)
{
	channel &= 0b00001111;			// AND with 16
	ADMUX = (ADMUX & 0xF0)			// Clear the bottom 4 bits
		  | channel;				// OR to select channel
}

void ADC_start_conversion(uint8_t channel)
{
	ADMUX &= ~0x0f;					// Clear bottom 4 bits
	ADMUX |= channel;				// Select channel
	ADCSRA |= (1 << ADSC);			// Start conversion
}

uint8_t ADC_is_conversion_done()
{
	return ((ADCSRA & (1 << ADIF)));
}

uint16_t ADC_get_conversion_result()
{
	return (ADCL | ADCH << 8);
	//return ADC;
}

uint16_t ADC_read(uint8_t channel)
{
	ADC_start_conversion(channel);
	while(ADCSRA & (1<<ADSC));		// wait while conversion finishes
	return ADC;
}