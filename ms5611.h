#ifndef MS5611_H
#define MS5611_H

#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "twi.h"
#include "my_errors.h"

uint32_t ms5611_get_data(float *, float *);

#endif