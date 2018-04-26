/*
 * AC Clamp Meter.c
 *
 * Created: 4/21/2018 1:19:46 PM
 * Author : peter
 * 
 */ 

#include "main.h"


#define MODE_IDLE 0
#define MODE_MEASURE 1
#define MODE_CALCULATE 2
#define MODE_DISPLAY 3

uint8_t adc_channel = 0;
volatile uint8_t mode = 0;
volatile uint16_t timer_ticks = 0;

int main(void)
{
	i_o_init();
	device_init();
	sei();
	
	uint16_t v_in = 0;
	uint_fast32_t sum = 0;
	volatile uint8_t samples = 40;
	volatile uint_fast32_t rms = 0;
	//uint8_t display_updates_per_second = 4;
	uint16_t display_update_interval = 600;
	uint16_t timer_ticks_to_display_update = 600; //magic number for now. 600 * 416 us = 0.25 s
	
	
	mode = MODE_IDLE;
	ADC_0_select_channel(adc_channel);
	
	while (1)
	{
		switch(mode)
		{
			/************************************************************************/
			/* This case is executed when ADC has finished sampling the input and	*/
			/* fired an interrupt.                                                  */
			/* Performs first part of RMS calculation: sum(x^2)						*/
			/* Decrementing counter keeps track how many samples remain             */
			/************************************************************************/
			case MODE_MEASURE:
				//PORTD |= (1 << INSTRUMENTATION_OUT);
				v_in = ADC_0_get_conversion_result();
				sum += pow(v_in, 2);
				samples -= 1;
				if(samples == 0) {
					mode = MODE_CALCULATE;
				} else {
					mode = MODE_IDLE;
				}
				//PORTD &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			/************************************************************************/
			/* This case is executed when all required samples have been taken.     */  
			/* Performs second part of RMS calculation: sqrt(sum/n)                 */                                                  
			/************************************************************************/	
			case MODE_CALCULATE:
				PORTD |= (1 << INSTRUMENTATION_OUT);
				rms = sum/40;
				rms = sqrt(rms);
				sum = 0;
				samples = 40;
				
				if (timer_ticks > timer_ticks_to_display_update) {
					mode = MODE_DISPLAY;
				} else {
					mode = MODE_IDLE;
				}
				PORTD &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			case MODE_DISPLAY:
				timer_ticks_to_display_update += display_update_interval;
				//send_two_bytes(rms);
				shift_out(rms);
				mode = MODE_IDLE;
				break;
				
			case MODE_IDLE:
				;
				break;
				
			default:
				PORTD |= (1 << ERROR);
		}
	}
	return 0;
}

void send_two_bytes(uint16_t data)
{
	uint8_t byte = 0;
	uint8_t number_of_bytes = 2;
	for(byte = 0; byte < number_of_bytes; byte++) {
		USART_0_write((data >> (byte*8)) & 0xff);
	}
}

void shift_out(uint16_t data)
{
	uint8_t bit = 0;
	uint8_t next_bit = 0;
	
	for (bit = 0; bit < 10; bit++) {
		next_bit = (data >> bit) & 0x01;
		
		if (next_bit) {
			PORTD |= (1 << DISPLAY);
		} else {
			PORTD &= ~(1 << DISPLAY);
		}
	}
	PORTD &= ~(1 << DISPLAY);
}

ISR(TIMER0_COMPA_vect)
{
	timer_ticks += 1;
	PORTD ^= (1 << ADC_CLK_OUT);
}

ISR(ADC_vect)
{
	mode = MODE_MEASURE;
}