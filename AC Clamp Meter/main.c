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


int main(void)
{
	i_o_init();
	device_init();
	sei();
	
	int num = 0;
	long int sum = 0;
	volatile int samples = 40;
	volatile uint_fast32_t rms = 0;
	
	mode = MODE_IDLE;
	ADC_select_channel(adc_channel);
	
	while (1)
	{
		switch(mode)
		{
			case MODE_MEASURE:
				//PORTD |= (1 << INSTRUMENTATION_OUT);
				//num = ADC_get_conversion(adc_channel);
				num = ADC_get_conversion_result();
				sum += (num * num);
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
				cli();
				rms = sum/40;
				rms = sqrt(rms);
				uint8_t i = 0;
				for(i=0; i<2; i++){
					USART_0_write((rms>>(i*8)) & 0xff);
				}
				sum = 0;
				samples = 40;
				sei();
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

ISR(TIMER0_COMPA_vect)
{
	//mode = MODE_MEASURE;
	PORTD ^= (1 << ADC_CLK_OUT);
}

ISR(ADC_vect)
{
	mode = MODE_MEASURE;
	//PORTD ^= (1 << INSTRUMENTATION_OUT);
}