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
    coef_struct.c0 = ((uint32_t)coeffs[0] << 4) | ((uint32_t)coeffs[1] >> 4);
    getTwosComplement(&coef_struct.c0, 12);
    coef_struct.c1 = (((uint32_t)coeffs[1] & 0x0F) << 8) | (uint32_t)coeffs[2];
    getTwosComplement(&coef_struct.c1, 12);
    coef_struct.c00 = ((uint32_t)coeffs[3] << 12) | ((uint32_t)coeffs[4] << 4) | ((uint32_t)coeffs[5] >> 4);
    getTwosComplement(&coef_struct.c00, 20);
    coef_struct.c10 = (((uint32_t)coeffs[5] & 0x0F) << 16) | ((uint32_t)coeffs[6] << 8) | (uint32_t)coeffs[7];
    getTwosComplement(&coef_struct.c10, 20);
    coef_struct.c01 = ((uint32_t)coeffs[8] << 8) | (uint32_t)coeffs[9];
    getTwosComplement(&coef_struct.c01, 16);
    coef_struct.c11 = ((uint32_t)coeffs[10] << 8) | (uint32_t)coeffs[11];
    getTwosComplement(&coef_struct.c11, 16);
    coef_struct.c20 = ((uint32_t)coeffs[12] << 8) | (uint32_t)coeffs[13];
    getTwosComplement(&coef_struct.c20, 16);
    coef_struct.c21 = ((uint32_t)coeffs[14] << 8) | (uint32_t)coeffs[15];
    getTwosComplement(&coef_struct.c21, 16);
    coef_struct.c30 = ((uint32_t)coeffs[16] << 8) | (uint32_t)coeffs[17];
    getTwosComplement(&coef_struct.c30, 16);
    coeffs_read = true;

    return err;
}

uint32_t
dps368_init(void)
{
    uint32_t err = 0;
    // Is sensor initialization successful?
    uint8_t init_succ = 0;
    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                   &init_succ, sizeof(init_succ));
    if (err)
        return err;
    if (!(init_succ & 0x40))
    {
        printf("DPS368 initialization failed\r\n");
        return init_failed;
    }
    if (coeffs_read == false)
    {
        err = dps368_get_coeffs();
        if (err)
            return err;
    }
    uint8_t coef_src = 0;
    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_COEF_SRCE,
                   &coef_src, sizeof(coef_src));
    if (err)
        return err;
    // Set precision for pressure measurement
    uint8_t indoor_prs_cfg = 0x04; // see datasheet, middle precision 16x
    err = twi_write(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_PRS_CFG,
                    &indoor_prs_cfg, sizeof(indoor_prs_cfg));
    if (err)
        return err;
    // Set precision for temperature measurement and determine temp coeff source, lowest precision 1x
    uint8_t indoor_temp_cfg = 0x00;
    if (coef_src & 0x80)
        indoor_temp_cfg = 0x80;
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
dps368_get_data(float *p, float *T)
{
    uint32_t err = 0;

    // Wait for sensor to become ready before reading data
    uint8_t snsr_ready = 0;
    //printf("Waiting for DPS368 to become ready\r\n");
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
    uint8_t temperature[3] = {0};
    nrf_delay_ms(5);
    //printf("Reading DPS368 Temperature data\r\n");
    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_TMP_BASE,
                   temperature, sizeof(temperature));
    if(err)
        return err;
    //printf("DPS368 Temperature data read\r\n");
    int32_t T_uncomp = ((uint32_t)temperature[0] << 16) | ((uint32_t)temperature[1] << 8) | (uint32_t)temperature[2];
    getTwosComplement(&T_uncomp, 24);
    // Get pressure data
    uint8_t start_prs_measurement = 0x01;
    err = twi_write(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                    &start_prs_measurement, sizeof(start_prs_measurement));
    if (err)
        return err;
    uint8_t pressure[3] = {0};
    nrf_delay_ms(40);
    uint8_t prs_ready = 0;
    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_MEAS_CFG,
                   &prs_ready, sizeof(prs_ready));
    if (!(prs_ready & 0x10))
        printf("PRESSURE MEASURMENT NOT YET READY\r\n");
    err = twi_read(TWI_INS_1, DPS368_I2C_ADDRESS, DPS368_PRS_BASE,
                   pressure, sizeof(pressure));
    if (err)
        return err;
    int32_t p_uncomp = ((uint32_t)pressure[0] << 16) | ((uint32_t)pressure[1] << 8) | (uint32_t)pressure[2];
    getTwosComplement(&p_uncomp, 24);
    //printf("DPS368 Uncomp data. Press: %ld, Temp: %ld\r\n", p_uncomp, T_uncomp);
    double Praw_sc = p_uncomp / SCALE_FACTOR_16X;
    double Traw_sc = T_uncomp / SCALE_FACTOR_1X;
    *T = coef_struct.c0 * 0.5 + coef_struct.c1 * Traw_sc;
    *p = coef_struct.c00 +
         Praw_sc * (coef_struct.c10 + Praw_sc * (coef_struct.c20 + Praw_sc * coef_struct.c30)) +
         Traw_sc * coef_struct.c01 +
         Traw_sc * Praw_sc * (coef_struct.c11 + Praw_sc * coef_struct.c21);
    return err;
}