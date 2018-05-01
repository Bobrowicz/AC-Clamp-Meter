/*
 * SPI_0.h
 *
 * Created: 4/26/2018 5:12:11 PM
 *  Author: peter
 */ 


#ifndef SPI_0_H_
#define SPI_0_H_

#define SCK PB5
#define MISO PB4
#define MOSI PB3
#define SS PB2

#include <avr/io.h>


/**
 * \brief Initialize SPI interface
 * If module is configured to disabled state, the clock to the SPI is disabled
 * if this is supported by the device's clock system.
 *
 * \return Nothing
 */
void SPI_0_init(void);

/**
 * \brief Enable SPI_0
 * 1. If supported by the clock system, enables the clock to the SPI
 * 2. Enables the SPI module by setting the enable-bit in the SPI control register
 *
 * \return Nothing
 */
void SPI_0_enable();

/**
 * \brief Disable SPI_0
 * 1. Disables the SPI module by clearing the enable-bit in the SPI control register
 * 2. If supported by the clock system, disables the clock to the SPI
 *
 * \return Nothing
 */
void SPI_0_disable();

void SPI_0_write(uint8_t);

#endif /* SPI_0_H_ */