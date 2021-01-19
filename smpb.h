#ifndef SMPB_H
#define SMPB_H

#include <stdint.h>
#include <math.h>
#include <stddef.h>

#include "twi.h"

#define SMPB_I2C_ADDRESS 0x70

// registers
#define TEMP_TXD0 0xfc
#define TEMP_TXD1 0xfb
#define TEMP_TXD2 0xfa
#define TEMP_BASE 0xFA
#define PRESS_TXD0 0xf9
#define PRESS_TXD1 0xf8
#define PRESS_TXD2 0xf7
#define PRESS_BASE 0xF7
#define IO_SETUP 0xf5
#define CTRL_MEAS 0xf4
#define DEVICE_STAT 0xd3
#define I2C_SET 0xf2
#define IIR_CNT 0xf1
#define RESET 0xe0
#define CHIP_ID 0xd1
#define SMPB_COE_END 0xB8
#define SMPB_COE_START 0xA0

// offset
#define COE_b00_a0_ex 24
#define COE_a2_0 23
#define COE_a2_1 22
#define COE_a1_0 21
#define COE_a1_1 20
#define COE_a0_0 19
#define COE_a0_1 18
#define COE_bp3_0 17
#define COE_bp3_1 16
#define COE_b21_0 15
#define COE_b21_1 14
#define COE_b12_0 13
#define COE_b12_1 12
#define COE_bp2_0 11
#define COE_bp2_1 10
#define COE_b11_0 9
#define COE_b11_1 8
#define COE_bp1_0 7
#define COE_bp1_1 6
#define COE_bt2_0 5
#define COE_bt2_1 4
#define COE_bt1_0 3
#define COE_bt1_1 2
#define COE_b00_0 1
#define COE_b00_1 0

#define SMPB_NUMBER_OF_COEFS 25

#define MODE_FORCED 0x1

#define AVG_SKIP 0x0
#define AVG_1    0x1
#define AVG_2    0x2
#define AVG_4    0x3
#define AVG_8    0x4
#define AVG_16   0x5
#define AVG_32   0x6
#define AVG_64   0x7

#define FILTER_OFF 0x0
#define FILTER_2   0x1
#define FILTER_4   0x2
#define FILTER_8   0x3
#define FILTER_16  0x4
#define FILTER_32  0x5

// conversion coefficients
#define A_a1 -0.0063
#define S_a1 0.00043
#define A_a2 -0.000000000019
#define S_a2 0.00000000012
#define A_bt1 0.1
#define S_bt1 0.091
#define A_bt2 0.000000012
#define S_bt2 0.0000012
#define A_bp1 0.033
#define S_bp1 0.019
#define A_b11 0.00000021
#define S_b11 0.00000014
#define A_bp2 -0.00000000063
#define S_bp2 0.00000000035
#define A_b12 0.00000000000029
#define S_b12 0.00000000000076
#define A_b21 0.0000000000000021
#define S_b21 0.000000000000012
#define A_bp3 0.00000000000000013
#define S_bp3 0.000000000000000079

struct smpb_coeffs {
    double b00;
    double bt1;
    double bt2;
    double bp1;
    double b11;
    double bp2;
    double b12;
    double b21;
    double bp3;
    double a0;
    double a1;
    double a2;
};

uint32_t smpb_init(void);
uint32_t smpb_get_data(double *, double *);

#endif