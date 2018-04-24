/*
 * tc0.c
 *
 * Created: 4/21/2018 4:50:43 PM
 *  Author: peter
 */ 
#include <TC_0.h>

void TIMER_0_init(void)
{
	TCCR0A |= (1 << WGM01); // Mode 2, Clear on Timer Compare
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler set to clk/64
	OCR0A = 103; // 2400 Hz
	TIMSK0 |= (1 << OCIE0A);	// Compare A Match Interrupt Enable
}