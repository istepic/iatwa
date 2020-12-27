#ifndef TWI_H
#define TWI_H

#include <stdint.h>
#include <stdio.h>

#include "nrf_drv_twi.h"
#include "boards.h"

#include "my_errors.h"

union Sample
{
    uint8_t byte[1];
    uint8_t two_bytes[2];
    uint8_t three_bytes[3];
};

uint32_t twi_init(void);
uint32_t twi_rx(const uint8_t, const uint8_t);
uint32_t twi_tx(const uint8_t, const uint8_t, uint8_t *, uint8_t);

#endif
