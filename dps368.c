#include "dps368.h"

static bool coeffs_read;
static struct dps368_coeffs coef_struct;

static uint32_t
dps368_get_coeffs(void)
{
    uint32_t err = 0;
    uint8_t coeffs[NUMBER_OF_COEF_REGISTERS] = {0};

    uint8_t coef_ready = 0;
    // Wait for coefficient registers to be ready for reading
    while ((coef_ready & 0x80) == 0)
    {
        err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS,
                       DPS368_MEAS_CFG, &coef_ready, sizeof(coef_ready));
        if (err)
            return err;
    }

    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS,
                DPS368_REG_COEF_START_ADDR, coeffs, sizeof(coeffs));
    if (err)
        return err;
    coef_struct.c0 = (coeffs[0] << 4 | coeffs[1] >> 4) - (2 << 11);
    coef_struct.c1 = (coeffs[1] & 0x0F) << 8 | coeffs[2];
    coef_struct.c00 = (coeffs[3] << 12 | coeffs[4] << 4 | coeffs[5] >> 4) - (2 << 19);
    coef_struct.c10 = ((coeffs[5] & 0x0F) << 16 | coeffs[6] << 8 | coeffs[7]) - (2 << 19);
    coef_struct.c01 = coeffs[8] << 8 | coeffs[9];
    coef_struct.c11 = coeffs[10] << 8 | coeffs[11];
    coef_struct.c20 = coeffs[12] << 8 | coeffs[13];
    coef_struct.c21 = coeffs[14] << 8 | coeffs[15];
    coef_struct.c30 = coeffs[16] << 8 | coeffs[17];
    coeffs_read = true;

    return err;
}

uint32_t
dps368_init(void)
{
    uint32_t err = 0;
    if (coeffs_read == false)
    {
        err = dps368_get_coeffs();
        if (err)
            return err;
    }
    // Set precision for pressure measurement
    uint8_t indoor_prs_cfg = 0x04; // see datasheet, middle precision
    err = twi_write(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_PRS_CFG,
                    &indoor_prs_cfg, sizeof(indoor_prs_cfg));
    if (err)
        return err;
    // Set precision for temperature measurement, lowest precision
    uint8_t indoor_temp_cfg = 0x00;
    err = twi_write(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_TMP_CFG,
                    &indoor_temp_cfg, sizeof(indoor_temp_cfg));
    if (err)
        return err;
    // Set main configuration register, only enable Pressure result bit shift
    uint8_t cfg_reg = 0x04;
    err = twi_write(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_CFG_REG,
                    &cfg_reg, sizeof(cfg_reg));
    if (err)
        return err;

    return err;
}

uint32_t
dps368_get_data(int32_t *p, int32_t *T)
{
    uint32_t err = 0;

    // Wait for sensor to become ready before reading data
    uint8_t snsr_ready = 0;
    while ((snsr_ready & 0x40) == 0)
    {
        err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                       &snsr_ready, sizeof(snsr_ready));
        if (err)
            return err;
    }

    // Get temperature data
    uint8_t start_tmp_measurement = 0x02;
    err = twi_write(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                    &start_tmp_measurement, sizeof(start_tmp_measurement));
    if (err)
        return err;
    uint8_t tmp_data_ready = 0;
    while ((tmp_data_ready & 0x20) == 0)
    {
        err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                       &tmp_data_ready, sizeof(tmp_data_ready));
        if (err)
            return err;
    }
    uint8_t temperature[3] = {0};
    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_TMP_BASE,
                   temperature, sizeof(temperature));
    if(err)
        return err;
    int32_t T_uncomp = (temperature[0] << 16 | temperature[1] << 8 | temperature[2]) - (2 << 23);

    // Get pressure data
    uint8_t start_prs_measurement = 0x01;
    err = twi_write(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                    &start_prs_measurement, sizeof(start_prs_measurement));
    if (err)
        return err;
    uint8_t prs_data_ready = 0;
    while ((prs_data_ready & 0x01) == 0)
    {
        err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                       &prs_data_ready, sizeof(prs_data_ready));
        if (err)
            return err;
    }
    uint8_t pressure[3] = {0};
    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_PRS_BASE,
                   pressure, sizeof(temperature));
    int32_t p_uncomp = (pressure[0] << 16 | pressure[1] << 8 | pressure[2]) - (2 << 23);

    int32_t Praw_sc = p_uncomp / SCALE_FACTOR_16X;
    int32_t Traw_sc = T_uncomp / SCALE_FACTOR_1X;
    *T = coef_struct.c0 * 0.5 + coef_struct.c1 * Traw_sc;
    *p = coef_struct.c00 +
         Praw_sc * (coef_struct.c10 + Praw_sc * (coef_struct.c20 + Praw_sc * coef_struct.c30)) +
         Traw_sc * coef_struct.c01 +
         Traw_sc * Praw_sc * (coef_struct.c11 + Praw_sc * coef_struct.c21);

    return err;
}