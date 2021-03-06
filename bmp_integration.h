#ifndef BMP_INTEGRATION_H
#define BMP_INTEGRATION_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "bmp3.h"
#include "twi.h"

#include "nrf_delay.h"

/* ++++++++++++++++++++++++++++++ Interface +++++++++++++++++++++++++++++++++ */
int8_t bmp388_get_data(struct bmp3_data *);
int8_t bmp388_init(void);

#endif
