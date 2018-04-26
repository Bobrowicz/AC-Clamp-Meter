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



void send_two_bytes(uint16_t);
void shift_out(uint16_t);

#endif /* MAIN_H_ */