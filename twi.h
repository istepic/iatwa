#ifndef TWI_H
#define TWI_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>

#include "nrf_drv_twi.h"
#include "boards.h"

#include "my_errors.h"

#define TWI_INS_0 0
#define TWI_INS_1 1

uint32_t twi_init(void);
uint32_t twi_rx(const uint8_t twi_ins,
                const uint8_t addr,
                uint8_t *data,
                size_t size);
uint32_t twi_tx(const uint8_t twi_ins,
                const uint8_t addr,
                const uint8_t *data,
                size_t size);
uint32_t twi_read(const uint8_t twi_ins,
                  const uint8_t twi_addr,
                  uint8_t cmd,
                  uint8_t *data,
                  size_t data_size);
uint32_t twi_write(const uint8_t twi_ins,
                   const uint8_t twi_addr,
                   uint8_t cmd,
                   const uint8_t *data,
                   size_t data_size);

#endif
