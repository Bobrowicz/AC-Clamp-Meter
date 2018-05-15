/*
 * main.h
 *
 * Created: 4/21/2018 6:22:47 PM
 *  Author: peter
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <clock_config.h>
#include <setup.h>
#include <util/delay.h>

typedef struct {
	uint8_t enable_pin;
	uint8_t bit_pattern;
} seven_segment_digit;




uint8_t encode_digit(uint8_t);
uint8_t mode_measure(uint32_t *);
uint8_t mode_calculate_rms(uint32_t *, uint16_t *);
uint8_t mode_update_display_buffer(seven_segment_digit *, uint16_t *);
uint8_t refresh_display(seven_segment_digit *);
void extract_digits(uint16_t, seven_segment_digit *);
float scale_output(uint16_t);
void shift_out(uint16_t);
//void send_digits(uint8_t *);
//void send_two_bytes(uint16_t);

#endif /* MAIN_H_ */