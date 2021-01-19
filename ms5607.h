#ifndef MS5607_H
#define MS5607_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "twi.h"
#include "my_errors.h"

// MS5607 device address
#define MS5607_ADDR 0x76

// MS5607 device commands
#define MS5607_RESET_COMMAND 0x1E
#define MS5607_START_PRESSURE_ADC_CONVERSION 0x48
#define MS5607_START_TEMPERATURE_ADC_CONVERSION 0x58
#define MS5607_READ_ADC 0x00

// MS5607 Conversion time delay
#define MS5607_CONVERSION_TIME_OSR_4096 10 // miliseconds

#define MS5607_PROM_READ_FROM_START 0xA0

#define MS5607_CRC_INDEX 7
#define MS5607_PRESSURE_SENSITIVITY_INDEX 1
#define MS5607_PRESSURE_OFFSET_INDEX 2
#define MS5607_TEMP_COEFF_OF_PRESSURE_SENSITIVITY_INDEX 3
#define MS5607_TEMP_COEFF_OF_PRESSURE_OFFSET_INDEX 4
#define MS5607_REFERENCE_TEMPERATURE_INDEX 5
#define MS5607_TEMP_COEFF_OF_TEMPERATURE_INDEX 6
#define MS5607_COEFFICIENT_NUMBERS 8

// Interface
uint32_t ms5607_get_data(int32_t *, int32_t *);

#endif
