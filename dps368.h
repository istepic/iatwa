#ifndef DPS368_H
#define DPS368_H

#include <stdint.h>
#include <stddef.h>

#include "twi.h"

#define DPS368_I2C_ADDRESS 0x77

#define DPS368_PRS_B2 0x00
#define DPS368_PRS_B1 0x01
#define DPS368_PRS_B0 0x02
#define DPS368_PRS_BASE 0x00
#define DPS368_TMP_B2 0x03
#define DPS368_TMP_B1 0x04
#define DPS368_TMP_B0 0x05
#define DPS368_TMP_BASE 0x03
#define DPS368_PRS_CFG 0x06
#define DPS368_TMP_CFG 0x07
#define DPS368_MEAS_CFG 0x08
#define DPS368_CFG_REG 0x09
#define DPS368_INT_STS 0x0A
#define DPS368_FIFO_STS 0x0A
#define DPS368_RESET 0x0C
#define DPS368_PRODUCT_ID 0x0D

#define DPS368_REG_COEF_START_ADDR 0x10
#define DPS368_COEF_C0 0x10
#define DPS368_COEF_C0C1 0x11
#define DPS368_COEF_C1 0x12
#define DPS368_COEF_C00_2 0x13
#define DPS368_COEF_C00_1 0x14
#define DPS368_COEF_C00C10 0x15
#define DPS368_COEF_C10_1 0x16
#define DPS368_COEF_C10_0 0x17
#define DPS368_COEF_C01_1 0x18
#define DPS368_COEF_C01_0 0x19
#define DPS368_COEF_C11_1 0x1A
#define DPS368_COEF_C11_0 0x1B
#define DPS368_COEF_C20_1 0x1C
#define DPS368_COEF_C20_0 0x1D
#define DPS368_COEF_C21_1 0x1E
#define DPS368_COEF_C21_0 0x1F
#define DPS368_COEF_C30_1 0x20
#define DPS368_COEF_C30_0 0x21
#define DPS368_REG_COEF_END_ADDR 0x21
#define DPS368_COEF_SRCE 0x28

#define NUMBER_OF_COEF_REGISTERS 18

#define SCALE_FACTOR_1X 524288
#define SCALE_FACTOR_16X 253952

struct dps368_coeffs {
    int16_t c0;
    int16_t c1;
    int32_t c00;
    int32_t c10;
    int16_t c01;
    int16_t c11;
    int16_t c20;
    int16_t c21;
    int16_t c30;
};

uint32_t dps368_init(void);
uint32_t dps368_get_data(int32_t *, int32_t *);

#endif
