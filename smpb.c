#include "smpb.h"

static struct smpb_coeffs coef_struct;
static bool coefs_read;

static uint32_t
smpb_get_coeffs(void)
{
    uint32_t err = 0;
    uint8_t coeffs[SMPB_NUMBER_OF_COEFS] = {0};

    err = twi_read(TWI_INS_0, SMPB_I2C_ADDRESS, SMPB_COE_START,
                   coeffs, sizeof(coeffs));
    if (err)
        return err;

    int32_t b00 = (coeffs[COE_b00_1] << 12 | coeffs[COE_b00_0] << 4 |
                  coeffs[COE_b00_a0_ex] >> 4) - (2 << 19);
    int16_t bt1 = coeffs[COE_bt1_1] << 8 | coeffs[COE_bt1_0];
    int16_t bt2 = coeffs[COE_bt2_1] << 8 | coeffs[COE_bt2_0];
    int16_t bp1 = coeffs[COE_bp1_1] << 8 | coeffs[COE_bp1_0];
    int16_t b11 = coeffs[COE_b11_1] << 8 | coeffs[COE_b11_0];
    int16_t bp2 = coeffs[COE_bp2_1] << 8 | coeffs[COE_bp2_0];
    int16_t b12 = coeffs[COE_b12_1] << 8 | coeffs[COE_b12_0];
    int16_t b21 = coeffs[COE_b21_1] << 8 | coeffs[COE_b21_0];
    int16_t bp3 = coeffs[COE_bp3_1] << 8 | coeffs[COE_bp3_0];
    int32_t a0 = (coeffs[COE_a0_1] << 12 | coeffs[COE_a0_0] << 4 |
                 coeffs[COE_b00_a0_ex] >> 4) - (2 << 19);
    int16_t a1 = coeffs[COE_a1_1] << 8 | coeffs[COE_a1_0];
    int16_t a2 = coeffs[COE_a2_1] << 8 | coeffs[COE_a2_0];

    coef_struct.b00 = b00 / 16;
    coef_struct.bt1 = A_bt1 + (double)(((S_bt1) * bt1) / 32767);
    coef_struct.bt2 = A_bt2 + (double)(((S_bt2) * bt2) / 32767);
    coef_struct.bp1 = A_bp1 + (double)(((S_bp1) * bp1) / 32767);
    coef_struct.b11 = A_b11 + (double)(((S_b11) * b11) / 32767);
    coef_struct.bp2 = A_bp2 + (double)(((S_bp2) * bp2) / 32767);
    coef_struct.b12 = A_b12 + (double)(((S_b12) * b12) / 32767);
    coef_struct.b21 = A_b21 + (double)(((S_b21) * b21) / 32767);
    coef_struct.bp3 = A_bp3 + (double)(((S_bp3) * bp3) / 32767);
    coef_struct.a0 = a0 / 16;
    coef_struct.a1 = A_a1 + (double)(((S_a1) * a1) / 32767);
    coef_struct.a2 = A_a2 + (double)(((S_a2) * a2) / 32767);
    coefs_read = true;
    return err;
}

uint32_t
smpb_init(void)
{
    uint32_t err = 0;
    uint8_t iir = 0x07; // N = 32

    if (coefs_read == false)
    {
        err = smpb_get_coeffs();
        if (err)
            return err;
    }
    // Write to IIR register
    err = twi_write(TWI_INS_0, SMPB_I2C_ADDRESS, IIR_CNT,
                    &iir, sizeof(iir));
    if (err)
        return err;
    return err;
}

uint32_t
smpb_get_data(double *p, double *T)
{
    uint32_t err = 0;
    uint8_t ctrl_meas = 0b01111001; // 32x pressure, 4x temperature, forced mode

    // Write to control register to set precision and start measurement
    err = twi_write(TWI_INS_0, SMPB_I2C_ADDRESS, CTRL_MEAS,
                    &ctrl_meas, sizeof(ctrl_meas));
    if (err)
        return err;

    uint8_t temperature[3] = {0};
    err = twi_read(TWI_INS_0, SMPB_I2C_ADDRESS, TEMP_BASE,
                   temperature, sizeof(temperature));
    if (err)
        return err;
    int32_t T_uncomp = (temperature[0] << 16 | temperature[1] << 8 | temperature[2]) - (2 << 23);

    uint8_t pressure[3] = {0};
    err = twi_read(TWI_INS_0, SMPB_I2C_ADDRESS, PRESS_BASE,
                   pressure, sizeof(pressure));
    if (err)
        return err;
    int32_t p_uncomp = (pressure[0] << 16 | pressure[1] << 8 | pressure[2]) - (2 << 23);

    double Tr = coef_struct.a0 +
                coef_struct.a1 * T_uncomp +
                coef_struct.a2 * (T_uncomp << 1);
    *p = coef_struct.b00 +
         coef_struct.bt1 * Tr +
         coef_struct.bp1 * p_uncomp +
         coef_struct.b11 * p_uncomp * Tr +
         coef_struct.bt2 * pow(Tr, 2) +
         coef_struct.bp2 * (p_uncomp << 1) +
         coef_struct.b12 * p_uncomp * pow(Tr, 2) +
         coef_struct.b21 * (p_uncomp << 1) * Tr +
         coef_struct.bp3 * pow(p_uncomp, 3);
    *T = Tr / 256;

    return err;
}