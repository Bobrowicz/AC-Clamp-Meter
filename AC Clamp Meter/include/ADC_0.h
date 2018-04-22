/*
 * ADC_0.h
 *
 * Created: 4/21/2018 2:07:04 PM
 *  Author: peter
 */ 


#ifndef ADC_0_H_
#define ADC_0_H_

#include <avr/io.h>

void ADC_init(void);
void ADC_enable();
void ADC_disable();
void ADC_clear_interrupt_flag(void);
void ADC_select_channel(uint8_t);
void ADC_start_conversion(uint8_t);
uint8_t ADC_is_conversion_done(void);
uint16_t ADC_get_conversion_result(void);
uint16_t ADC_get_conversion(uint8_t);


#endif /* ADC_0_H_ */