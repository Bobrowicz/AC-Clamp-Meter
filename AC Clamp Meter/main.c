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


int main(void)
{
	i_o_init();
	device_init();
	sei();
	
	uint16_t v_in = 0;
	uint_fast32_t sum = 0;
	volatile int samples = 40;
	volatile uint_fast32_t rms = 0;
	
	mode = MODE_IDLE;
	ADC_0_select_channel(adc_channel);
	
	while (1)
	{
		switch(mode)
		{
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
				
			case MODE_CALCULATE:
				PORTD |= (1 << INSTRUMENTATION_OUT);
				//cli();
				rms = sum/40;
				rms = sqrt(rms);
				send_two_bytes(rms);
				sum = 0;
				samples = 40;
				//sei();
				mode = MODE_IDLE;
				PORTD &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			case MODE_DISPLAY:
				// nothing yet
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

void send_two_bytes(uint16_t bytes)
{
	uint8_t i = 0;
	for(i = 0; i < 2; i++){
		USART_0_write((bytes >> (i*8)) & 0xff);
	}
}

ISR(TIMER0_COMPA_vect)
{
	PORTD ^= (1 << ADC_CLK_OUT);
}

ISR(ADC_vect)
{
	mode = MODE_MEASURE;
}