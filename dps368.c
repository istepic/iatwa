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
        err = twi_read(TWI_INS_0, DPS368_I2C_ADDRESS,
                       DPS368_MEAS_CFG, coef_ready, sizeof(coef_ready));
        if (err)
            return err;
        nrf_delay_ms(5);
    }

    size_t count = 0;
    for (uint8_t reg = DPS368_REG_COEF_START_ADDR;
         reg <= DPS368_REG_COEF_END_ADDR; ++reg)
    {
        err = twi_read(TWI_INS_0, DPS368_I2C_ADDRESS,
                 reg, coeffs[count], sizeof(uint8_t));
        if (err)
            return err;
        ++count;
    }
    coef_struct.c0 = coeffs[0] << 4 | coeffs[1] >> 4;
    coef_struct.c1 = (coeffs[1] & 0x0F) << 8 | coeffs[2];
    coef_struct.c00 = coeffs[3] << 12 | coeffs[4] << 4 | coeffs[5] >> 4;
    coef_struct.c10 = (coeffs[5] & 0x0F) << 16 | coeffs[6] << 8 | coeffs[7];
    coef_struct.c01 = coeffs[8] << 8 | coeffs[9];
    coef_struct.c11 = coeffs[10] << 8 | coeffs[11];
    coef_struct.c20 = coeffs[12] << 8 | coeffs[13];
    coef_struct.c21 = coeffs[14] << 8 | coeffs[15];
    coef_struct.c30 = coeffs[16] << 8 | coeffs[17];
    coeffs_read = true;
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
    // Set precision for temperature measurement, low precision
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
}

uint32_t
dps368_get_data(uint32_t *p, uint32_t *T)
{
    uint32_t err = 0;

    // Wait for sensor to become ready before reading data
    uint8_t snsr_ready = 0;
    while ((snsr_ready & 0x40) == 0)
    {
        err = twi_read(TWI_INS_0, DPS368_I2C_ADDRESS,
                       DPS368_MEAS_CFG, snsr_ready, sizeof(snsr_ready));
        if (err)
            return err;
        nrf_delay_ms(5);
    }

    err = twi_write(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                    DPS368_START_TMP_MEAS, sizeof(uint8_t));
    if (err)
        return err;
    uint8_t temperature[3] = {0};
    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_START_TMP_MEAS,
                   temperature, sizeof(temperature));
    uint8_t pressure[3] = {0};
}