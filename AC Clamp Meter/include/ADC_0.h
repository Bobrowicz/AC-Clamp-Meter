/*
 * ADC_0.h
 *
 * Created: 4/21/2018 2:07:04 PM
 * Author: peter
 * Most of the code is from Atmel Start
 */ 


#ifndef ADC_0_H_
#define ADC_0_H_

#include <avr/io.h>

/**
 * \brief Initialize ADC interface
 * If module is configured to disabled state, the clock to the ADC is disabled
 * if this is supported by the device's clock system.
 *
 * \return Initialization status.
 * \retval 0 the ADC init was successful
 * \retval 1 the ADC init was not successful
 */
uint8_t ADC_0_init(void);

/**
 * \brief Enable ADC_0
 * 1. If supported by the clock system, enables the clock to the ADC
 * 2. Enables the ADC module by setting the enable-bit in the ADC control register
 *
 * \return Nothing
 */
void ADC_0_enable(void);

/**
 * \brief Disable ADC_0
 * 1. Disables the ADC module by clearing the enable-bit in the ADC control register
 * 2. If supported by the clock system, disables the clock to the ADC
 *
 * \return Nothing
 */
void ADC_0_disable(void);

/**
 * \brief Write 1 to ADIF register to clear interrupt flag
 *
 * \return Nothing
 */
void ADC_0_clear_interrupt_flag(void);

/**
 * \brief Write to ADMUX registers to select ADC channel
 *
 * \return Nothing
 */
void ADC_0_select_channel(uint8_t);

/**
 * \brief Start a conversion on ADC_0
 *
 * \param[in] channel The ADC channel to start conversion on
 *
 * \return Nothing
 */
void ADC_0_start_conversion(uint8_t);

/**
 * \brief Check if the ADC conversion is done
 *
 * \return The status of ADC conversion done check
 * \retval true The ADC conversion is done
 * \retval false The ADC conversion is not done
 */
uint8_t ADC_0_is_conversion_done(void);

/**
 * \brief Read a conversion result from ADC_0
 *
 * \return Conversion result read from the ADC_0 ADC module
 */
uint16_t ADC_0_get_conversion_result(void);

/**
 * \brief Start a conversion, wait until ready, and return the conversion result
 *
 * \return Conversion result read from the ADC_0 ADC module
 */
uint16_t ADC_0_get_conversion(uint8_t);


#endif /* ADC_0_H_ */