/*
 * tc2.c
 *
 * Created: 4/21/2018 1:21:27 PM
 *  Author: peter
 */ 
#include "tc2.h"

void timer_2_init(void)
{
	TCCR2A |= (1 << WGM21); // Mode 2, Clear on Timer Compare
	TCCR2B |= (1 << CS21) | (1 << CS20); // Prescaler set to clk/32
	OCR2A = 208; // 2400 Hz
	TIMSK2 |= (1 << OCIE2B);
}