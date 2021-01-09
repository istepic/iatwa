#ifndef MS5607_H
#define MS5607_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "twi.h"
#include "my_errors.h"

// Interface
uint32_t ms5607_get_data(int32_t *, int32_t *);

#endif
