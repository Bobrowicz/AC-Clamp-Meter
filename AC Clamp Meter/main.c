/*
 * AC Clamp Meter.c
 *
 * Created: 4/21/2018 1:19:46 PM
 * Author : peter
 * 
 */ 

#include "main.h"


#define MODE_SYNC 0
#define MODE_MEASURE 1
#define MODE_CALCULATE 2
#define MODE_UPDATE_DISPLAY 3
#define MODE_REFRESH_DISPLAY 4
#define MODE_DEBUG 5

uint8_t adc_channel = 0;
volatile uint8_t mode = 0;
volatile uint16_t timer_ticks = 0;
volatile uint8_t disp_timer = 0;

static const float scale_factor = 0.1;

int main(void)
{
	_delay_ms(1000);
	i_o_init();
	device_init();
	ADC_0_select_channel(adc_channel);
	
	uint16_t timer_ticks_to_display_update = 600; // magic number for now. 600 * 416 us = 0.25 s
	uint8_t display_refresh_ticks = 15;	// 15 * 416 us = 6.25 ms, higher values cause flicker
	
	seven_segment_digit tenths_pace = {.enable_pin = DISPLAY_DIGIT_TENTHS};
	seven_segment_digit ones_place = {.enable_pin = DISPLAY_DIGIT_ONES};
	seven_segment_digit tens_place = {.enable_pin = DISPLAY_DIGIT_TENS};
	seven_segment_digit digits[3] = {tenths_pace, ones_place, tens_place};
		
	uint32_t sum = 0;
	uint16_t rms = 0;
		
	sei();

	mode = MODE_SYNC;
	
	while (1)
	{
		
		switch(mode)
		{
			
			case MODE_SYNC:
				if (timer_ticks > timer_ticks_to_display_update) {
					mode = MODE_UPDATE_DISPLAY;
					timer_ticks = 0;
				}
				
				if (disp_timer > display_refresh_ticks) {
					disp_timer = 0;
					mode = MODE_REFRESH_DISPLAY;
				}
				break;
			
			/************************************************************************/
			/* This case is executed when ADC has finished sampling the input and	*/
			/* issued an interrupt.                                                 */
			/************************************************************************/
			case MODE_MEASURE:
				//PORTC |= (1 << INSTRUMENTATION_OUT);
				mode = mode_measure(&sum);
				//PORTC &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			/************************************************************************/
			/* This case is executed when all required samples have been taken.     */                                                    
			/************************************************************************/	
			case MODE_CALCULATE:
				//PORTC |= (1 << INSTRUMENTATION_OUT);
				mode = mode_calculate_rms(&sum, &rms);
				PORTC &= ~(1 << INSTRUMENTATION_OUT);
				break;
			
			/************************************************************************/
			/* This case is executed approximately every 0.25 seconds.              */
			/* Updates buffer holding digits displayed on 7-segment                 */
			/************************************************************************/	
			case MODE_UPDATE_DISPLAY:
				//PORTC |= (1 << INSTRUMENTATION_OUT);
				mode = mode_update_display_buffer(digits, &rms);
				//PORTC &= ~(1 << INSTRUMENTATION_OUT);
				break;
				
			/************************************************************************/
			/* This case is executed every 6.25 ms.									*/
			/* Visually determined this to be maximum refresh interval that does	*/
			/* not produce flicker.                                                 */
			/************************************************************************/
			case MODE_REFRESH_DISPLAY:
				//PORTC |= (1 << INSTRUMENTATION_OUT);
				mode = refresh_display(digits);
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

uint8_t mode_measure(uint32_t *sum)
{
	static uint8_t samples = 40;
	int16_t adc_reading;
	
	adc_reading = ADC_0_get_conversion_result() - 511;
	*sum += pow(adc_reading, 2);
	samples -= 1;
	if(samples == 0) {
		samples = 40;
		return MODE_CALCULATE;
		} else {
		return MODE_SYNC;
	}
}

uint8_t mode_calculate_rms(uint32_t *sum, uint16_t *rms)
{
	*rms = sqrt(*sum / 40);
	*sum = 0;
	return MODE_SYNC;
}

uint8_t mode_update_display_buffer(seven_segment_digit *digits, uint16_t *rms)
{
	uint16_t output_value = scale_output(*rms);
	extract_digits(output_value, digits);
	return MODE_SYNC;
}

uint8_t refresh_display(seven_segment_digit *dig)
{
	static uint8_t digit = 0;
	
	PORTD &= ~(1 << dig[digit].enable_pin); // turn off power to current digit
	PORTD &= ~(1 << DISPLAY_CLR);	// clear shift register - active low
	PORTD |= (1 << DISPLAY_CLR);	
	
	++digit;	// select next digit
	if (digit > 2) {
		digit = 0;
	}
	
	SPI_0_write(dig[digit].bit_pattern);  // write data to shift register
	PORTD |= (1 << dig[digit].enable_pin); // turn on power to the next digit
	return MODE_SYNC;
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

void send_two_bytes(uint16_t data)
{
	uint8_t byte = 0;
	uint8_t number_of_bytes = 2;
	for(byte = 0; byte < number_of_bytes; byte++) {
		USART_0_write((data >> (byte*8)) & 0xff);
		//SPI_0_write((data >> (byte*8)) & 0xff);
	}
}
*/
float scale_output(uint16_t adc_reading)
{
	return (round(adc_reading * scale_factor));
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