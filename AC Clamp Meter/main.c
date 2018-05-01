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
#define MODE_DEBUG 4

uint8_t adc_channel = 0;
volatile uint8_t mode = 0;
volatile uint16_t timer_ticks = 0;

static const float scale_factor = 0.9766;



int main(void)
{
	i_o_init();
	device_init();
	sei();
	
	uint8_t digits[3];
	uint16_t adc_reading = 0;
	uint32_t sum = 0;
	volatile uint8_t samples = 40;
	volatile uint32_t rms = 0;
	uint16_t display_update_interval = 600;
	uint16_t timer_ticks_to_display_update = 600; // magic number for now. 600 * 416 us = 0.25 s
	
	uint8_t d = 0;
	
	mode = MODE_IDLE;
	ADC_0_select_channel(adc_channel);
	
	while (1)
	{
		switch(mode)
		{
			case MODE_IDLE:
			;
			break;
			
			/************************************************************************/
			/* This case is executed when ADC has finished sampling the input and	*/
			/* fired an interrupt.                                                  */
			/* Performs first part of RMS calculation: sum(x^2)						*/
			/* Decrementing counter keeps track how many samples remain             */
			/************************************************************************/
			case MODE_MEASURE:
				//PORTD |= (1 << INSTRUMENTATION_OUT);
				adc_reading = ADC_0_get_conversion_result();
				sum += pow(adc_reading, 2);
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
				//PORTD |= (1 << INSTRUMENTATION_OUT);
				rms = sum / 40;
				rms = sqrt(rms);
				sum = 0;
				samples = 40;
				
				if (timer_ticks > timer_ticks_to_display_update) {
					mode = MODE_DISPLAY;
				} else {
					mode = MODE_IDLE;
				}
				//PORTD &= ~(1 << INSTRUMENTATION_OUT);
				break;
			
			/************************************************************************/
			/* This case is executed approximately every 0.25 seconds.              */
			/* Sends latest calculated value to the 7-segment display.              */
			/************************************************************************/	
			case MODE_DISPLAY:
				PORTD |= (1 << INSTRUMENTATION_OUT);
				timer_ticks_to_display_update += display_update_interval;
				
				uint16_t output_value = scale_output(adc_reading);
				
				extract_digits(output_value, digits);
				send_digits(digits);
				
				mode = MODE_IDLE;
				PORTD &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			
			case MODE_DEBUG:
				
				PORTD |= (1 << INSTRUMENTATION_OUT);
				PORTD &= ~(1 << DISPLAY_CLR);
				//_delay_us(1);
				PORTD |= (1 << DISPLAY_CLR);
				SPI_0_write(encode_digit(d));
				
				d += 1;
				if (d > 9) {
					d = 0;
				}
				mode = MODE_IDLE;
				PORTD &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			default:
				PORTD |= (1 << ERROR);
		}
	}
	return 0;
}

uint8_t encode_digit(uint8_t digit)
{
	static const uint8_t lookup[10] = {
		0x3f, // 0
		0x06, // 1
		0x5b, // 2
		0x4f, // 3
		0x66, // 4
		0x6d, // 5
		0x7d, // 6
		0x07, // 7
		0x7f, // 8
		0x67  // 9
	};
	digit = lookup[digit];
	return digit;
}

void extract_digits(uint16_t number, uint8_t *digits)
{
	uint8_t i = 2;
	while (number) {
		digits[i] = number % 10;
		number = number / 10;
		i--;
	}
}

void send_digits(uint8_t *digits) 
{
	uint8_t i = 0;
	for (i = 0; i < 3; i++) {
		SPI_0_write(digits[i]);
		//SPI_0_write(encode_digit(digits[i]));
	}
}

float scale_output(uint16_t adc_reading)
{
	return (round(adc_reading * scale_factor));
}

void send_two_bytes(uint16_t data)
{
	uint8_t byte = 0;
	uint8_t number_of_bytes = 2;
	for(byte = 0; byte < number_of_bytes; byte++) {
		//USART_0_write((data >> (byte*8)) & 0xff);
		SPI_0_write((data >> (byte*8)) & 0xff);
	}
}
/*
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
*/

ISR(TIMER0_COMPA_vect)
{
	//timer_ticks += 1;
	
	static uint16_t timer_ticks_to_display_update = 512;
	timer_ticks += 1;
	if (timer_ticks > timer_ticks_to_display_update) {
		mode = MODE_DEBUG;
		timer_ticks = 0;
		} else {
		mode = MODE_IDLE;
	}
	PORTD ^= (1 << ADC_CLK_OUT);
}

ISR(ADC_vect)
{
	//mode = MODE_MEASURE;
}