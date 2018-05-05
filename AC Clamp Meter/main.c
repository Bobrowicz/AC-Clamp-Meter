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
#define MODE_UPDATE_DISPLAY 3
#define MODE_DEBUG 4

uint8_t adc_channel = 0;
volatile uint8_t mode = 0;
volatile uint16_t timer_ticks = 0;
volatile uint8_t disp_timer = 0;

static const float scale_factor = 0.9766;

int main(void)
{
	i_o_init();
	device_init();
	ADC_0_select_channel(adc_channel);
	
	
	uint16_t adc_reading = 0;
	uint32_t sum = 0;
	volatile uint8_t samples = 40;
	volatile uint32_t rms = 0;

	uint16_t timer_ticks_to_display_update = 600; // magic number for now. 600 * 416 us = 0.25 s
	uint8_t display_refresh_ticks = 15;
	
	seven_segment_digit tenths_pace = {.enable_pin = DISPLAY_DIGIT_TENTHS};
	seven_segment_digit ones_place = {.enable_pin = DISPLAY_DIGIT_ONES};
	seven_segment_digit tens_place = {.enable_pin = DISPLAY_DIGIT_TENS};
	seven_segment_digit digits_[3] = {tenths_pace, tens_place, ones_place };
		
	sei();

	mode = MODE_IDLE;
	_delay_ms(1000);
	
	while (1)
	{
		
		switch(mode)
		{
			
			case MODE_IDLE:
				if (timer_ticks > timer_ticks_to_display_update) {
					mode = MODE_UPDATE_DISPLAY;
					timer_ticks = 0;
				}
				
				if (disp_timer > display_refresh_ticks) {
					disp_timer = 0;
					display_refresh(digits_);
				}
				break;
			
			/************************************************************************/
			/* This case is executed when ADC has finished sampling the input and	*/
			/* fired an interrupt.                                                  */
			/* Performs first part of RMS calculation: sum(x^2)						*/
			/* Decrementing counter keeps track how many samples remain             */
			/************************************************************************/
			case MODE_MEASURE:
				PORTC |= (1 << INSTRUMENTATION_OUT);
				adc_reading = ADC_0_get_conversion_result();
				sum += pow(adc_reading, 2);
				samples -= 1;
				if(samples == 0) {
					mode = MODE_CALCULATE;
				} else {
					mode = MODE_IDLE;
				}
				PORTC &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			/************************************************************************/
			/* This case is executed when all required samples have been taken.     */  
			/* Performs second part of RMS calculation: sqrt(sum/n)                 */                                                  
			/************************************************************************/	
			case MODE_CALCULATE:
				//PORTC |= (1 << INSTRUMENTATION_OUT);
				rms = sum / 40;
				rms = sqrt(rms);
				sum = 0;
				samples = 40;
				mode = MODE_IDLE;
				//PORTC &= ~(1 << INSTRUMENTATION_OUT);
				break;
			
			/************************************************************************/
			/* This case is executed approximately every 0.25 seconds.              */
			/* Updates buffer holding digits displayed on 7-segment                 */
			/************************************************************************/	
			case MODE_UPDATE_DISPLAY:
				;
				//PORTC |= (1 << INSTRUMENTATION_OUT);
				uint16_t output_value = scale_output(rms);
				
				extract_digits(output_value, digits_);
				
				mode = MODE_IDLE;
				//PORTC &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			case MODE_DEBUG:
				//PORTC |= (1 << INSTRUMENTATION_OUT);
				
				//PORTC &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			default:
				;
		}
	}
	return 0;
}

void display_refresh(seven_segment_digit *dig)
{
	static uint8_t d = 0;
	//PORTC |= (1 << INSTRUMENTATION_OUT);
	
	PORTD &= ~(1 << dig[d].enable_pin); // turn off power to current digit
	
	
	PORTD &= ~(1 << DISPLAY_CLR);
	PORTD |= (1 << DISPLAY_CLR);	// clear shift register
	
	++d;	// select next digit
	if (d > 2) {
		d = 0;
	}
	
	SPI_0_write(dig[d].bit_pattern);  // write data to shift register
	
	PORTD |= (1 << dig[d].enable_pin); // turn on power to the next digit
	//mode = MODE_IDLE;
	
	//PORTC &= ~(1 << INSTRUMENTATION_OUT);
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

void extract_digits(uint16_t number, seven_segment_digit *digits)
{
	uint8_t i = 0;
	while (i < 3) {
		digits[i].bit_pattern = encode_digit(number % 10);
		number = number / 10;
		i++;
	}
}
/*
void send_digits(uint8_t *digits) 
{
	uint8_t i = 0;
	for (i = 0; i < 3; i++) {
		SPI_0_write(digits[i]);
		//SPI_0_write(encode_digit(digits[i]));
	}
}
*/
float scale_output(uint16_t adc_reading)
{
	return (round(adc_reading * scale_factor));
}

void send_two_bytes(uint16_t data)
{
	uint8_t byte = 0;
	uint8_t number_of_bytes = 2;
	for(byte = 0; byte < number_of_bytes; byte++) {
		USART_0_write((data >> (byte*8)) & 0xff);
		//SPI_0_write((data >> (byte*8)) & 0xff);
	}
}

ISR(TIMER0_COMPA_vect)
{
	timer_ticks += 1;
	disp_timer += 1;
	PORTC ^= (1 << TIMER_OUT);
}

ISR(ADC_vect)
{
	mode = MODE_MEASURE;
}