/*
 * ADC_0.c
 *
 * Created: 4/21/2018 2:07:20 PM
 *  Author: peter
 */ 

#include <ADC_0.h>

uint8_t ADC_0_init()
{
	ADMUX  = (1 << REFS0)			// Voltage Reference Selection: AVcc
		   | (0 << ADLAR)			// Left Adjust Result: disabled
		   | (0 << MUX0);			// Analog Channel Selection: Single Ended Input pin 0
		   
	ADCSRA  = (1 << ADEN)			// ADC Enable: enabled
			| (1 << ADATE)			// ADC Auto Trigger: enabled
			| (1 << ADIE)			// ADC Interrupt: enabled
	        | (1 << ADPS2) 
	        | (1 << ADPS1) 
	        | (1 << ADPS0);		    // ADC Prescaler Select: clk/128
	
	ADCSRB  = (0 << ADTS2)
			| (1 << ADTS1)
			| (1 << ADTS0);			// ADC Auto Trigger Source: TC0 Compare Match A
			
	return 0;
}

void ADC_0_enable()
{
	ADCSRA  |= (1 << ADEN);			// Enable ADC
}

void ADC_0_disable()
{
	ADCSRA  &= ~(1 << ADEN);		// Disable ADC
}

void ADC_0_clear_interrupt_flag()
{
	ADCSRA |= (1 << ADIF);			// Write 1 to flag to clear
}

void ADC_0_select_channel(uint8_t channel)
{
	channel &= 0b00001111;			// AND with 16
	ADMUX = (ADMUX & 0xF0)			// Clear the bottom 4 bits
		  | channel;				// OR to select channel
}

void ADC_0_start_conversion(uint8_t channel)
{
	ADMUX &= ~0x0f;					// Clear bottom 4 bits
	ADMUX |= channel;				// Select channel
	ADCSRA |= (1 << ADSC);			// Start conversion
}

uint8_t ADC_0_is_conversion_done()
{
	return ((ADCSRA & (1 << ADIF)));
}

uint16_t ADC_0_get_conversion_result()
{
	ADC_0_clear_interrupt_flag();
	return (ADCL | ADCH << 8);
}

uint16_t ADC_0_get_conversion(uint8_t channel)
{
	ADC_0_start_conversion(channel);
	while(ADCSRA & (1<<ADSC));		// wait while conversion finishes
	return ADC_0_get_conversion_result();
}