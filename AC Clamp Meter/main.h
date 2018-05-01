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

uint8_t encode_digit(uint8_t);
void extract_digits(uint16_t, uint8_t *);
void send_digits(uint8_t *);
float scale_output(uint16_t);
void send_two_bytes(uint16_t);
void shift_out(uint16_t);

#endif /* MAIN_H_ */