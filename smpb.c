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

    int32_t b00 = coeffs[COE_b00_1] << 12 | coeffs[COE_b00_0] << 4 |
                  coeffs[COE_b00_a0_ex] >> 4;
    getTwosComplement(&b00, 20);
    int32_t bt1 = coeffs[COE_bt1_1] << 8 | coeffs[COE_bt1_0];
    getTwosComplement(&bt1, 16);
    int32_t bt2 = coeffs[COE_bt2_1] << 8 | coeffs[COE_bt2_0];
    getTwosComplement(&bt2, 16);
    int32_t bp1 = coeffs[COE_bp1_1] << 8 | coeffs[COE_bp1_0];
    getTwosComplement(&bp1, 16);
    int32_t b11 = coeffs[COE_b11_1] << 8 | coeffs[COE_b11_0];
    getTwosComplement(&b11, 16);
    int32_t bp2 = coeffs[COE_bp2_1] << 8 | coeffs[COE_bp2_0];
    getTwosComplement(&bp2, 16);
    int32_t b12 = coeffs[COE_b12_1] << 8 | coeffs[COE_b12_0];
    getTwosComplement(&b12, 16);
    int32_t b21 = coeffs[COE_b21_1] << 8 | coeffs[COE_b21_0];
    getTwosComplement(&b21, 16);
    int32_t bp3 = coeffs[COE_bp3_1] << 8 | coeffs[COE_bp3_0];
    getTwosComplement(&bp3, 16);
    int32_t a0 = coeffs[COE_a0_1] << 12 | coeffs[COE_a0_0] << 4 |
                 coeffs[COE_b00_a0_ex] >> 4;
    getTwosComplement(&a0, 20);
    int32_t a1 = coeffs[COE_a1_1] << 8 | coeffs[COE_a1_0];
    getTwosComplement(&a1, 16);
    int32_t a2 = coeffs[COE_a2_1] << 8 | coeffs[COE_a2_0];
    getTwosComplement(&a2, 16);

    //printf("2SMPB Koeficjenti: b00: %ld bt1: %ld bt2: %ld bp1: %ld b11: %ld bp2: %ld b12: %ld b21: %ld bp3: %ld a0: %ld a1: %ld a2: %ld\r\n", b00, bt1, bt2, bp1, b11, bp2, b12, b21, bp3, a0, a1, a2);

    coef_struct.b00 = (double)b00 / 16;
    coef_struct.bt1 = (double)A_bt1 + (double)(((double)(S_bt1) * (double)bt1) / 32767);
    coef_struct.bt2 = (double)A_bt2 + (double)(((double)(S_bt2) * (double)bt2) / 32767);
    coef_struct.bp1 = (double)A_bp1 + (double)(((double)(S_bp1) * (double)bp1) / 32767);
    coef_struct.b11 = (double)A_b11 + (double)(((double)(S_b11) * (double)b11) / 32767);
    coef_struct.bp2 = (double)A_bp2 + (double)(((double)(S_bp2) * (double)bp2) / 32767);
    coef_struct.b12 = (double)A_b12 + (double)(((double)(S_b12) * (double)b12) / 32767);
    coef_struct.b21 = (double)A_b21 + (double)(((double)(S_b21) * (double)b21) / 32767);
    coef_struct.bp3 = (double)A_bp3 + (double)(((double)(S_bp3) * (double)bp3) / 32767);
    coef_struct.a0 = (double)a0 / 16;
    coef_struct.a1 = (double)A_a1 + (double)(((double)(S_a1) * (double)a1) / 32767);
    coef_struct.a2 = (double)A_a2 + (double)(((double)(S_a2) * (double)a2) / 32767);
    // if (coef_struct.b00  != 0 && coef_struct.bt1  != 0 && coef_struct.bt2  != 0 && coef_struct.bp1  != 0 && coef_struct.b11  != 0 && coef_struct.bp2  != 0 && coef_struct.b12  != 0 && coef_struct.b21  != 0 && coef_struct.bp3  != 0 && coef_struct.a0  != 0 && coef_struct.a1  != 0 && coef_struct.a2 != 0)
    //     printf("NULA JE\r\n");
    // printf("2SMPB Koeficjenti: b00: %f bt1: %f bt2: %f bp1: %f b11: %f bp2: %f b12: %f b21: %f bp3: %f a0: %f a1: %f a2: %f\r\n", coef_struct.b00, coef_struct.bt1, coef_struct.bt2, coef_struct.bp1, coef_struct.b11, coef_struct.bp2, coef_struct.b12, coef_struct.b21, coef_struct.bp3, coef_struct.a0, coef_struct.a1, coef_struct.a2);
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
    nrf_delay_ms(40);
    uint8_t temperature[3] = {0};
    err = twi_read(TWI_INS_0, SMPB_I2C_ADDRESS, TEMP_BASE,
                   temperature, sizeof(temperature));
    if (err)
        return err;
    int32_t T_uncomp = (temperature[0] << 16 | temperature[1] << 8 | temperature[2]) - (1 << 23);

    uint8_t pressure[3] = {0};
    err = twi_read(TWI_INS_0, SMPB_I2C_ADDRESS, PRESS_BASE,
                   pressure, sizeof(pressure));
    if (err)
        return err;
    int32_t p_uncomp = (pressure[0] << 16 | pressure[1] << 8 | pressure[2]) - (1 << 23);

    double Tr = (double)coef_struct.a0 +
                (double)coef_struct.a1 * (double)T_uncomp +
                (double)coef_struct.a2 * (double)pow((double)T_uncomp, 2);
    *p = (double)coef_struct.b00 +
         (double)coef_struct.bt1 * (double)Tr +
         (double)coef_struct.bp1 * (double)p_uncomp +
         (double)coef_struct.b11 * (double)p_uncomp * (double)Tr +
         (double)coef_struct.bt2 * (double)pow((double)Tr, 2) +
         (double)coef_struct.bp2 * (double)pow((double)p_uncomp, 2) +
         (double)coef_struct.b12 * (double)((double)p_uncomp * pow((double)Tr, 2)) +
         (double)coef_struct.b21 * (double)pow((double)p_uncomp, 2) * (double)Tr +
         (double)coef_struct.bp3 * (double)pow(p_uncomp, 3);
    *T = (double)Tr / 256;

    return err;
}