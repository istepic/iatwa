#include "bmp_integration.h"

static int8_t bmp388_twi_read(uint8_t reg_addr, uint8_t *reg_data,
                              uint32_t len, void *intf_ptr);
static int8_t bmp388_twi_write(uint8_t reg_addr, const uint8_t *reg_data,
                               uint32_t len, void *intf_ptr);
static void bmp388_delay_us(uint32_t period, void *intf_ptr);

static struct bmp3_dev dev;

/* +++++++++ Read, Write and Delay functions for "struct bmp3_dev" ++++++++++ */

static int8_t
bmp388_twi_read(uint8_t reg_addr, uint8_t *reg_data,
                uint32_t len, void *intf_ptr)
{
    uint32_t err = 0;
    err = twi_read(TWI_INS_0, *(const uint8_t *)intf_ptr, reg_addr,
                   reg_data, len);
    return err;
}

static int8_t
bmp388_twi_write(uint8_t reg_addr, const uint8_t *reg_data,
                 uint32_t len, void *intf_ptr)
{
    uint32_t err = 0;
    err = twi_write(TWI_INS_0, *(const uint8_t *)intf_ptr, reg_addr,
                    reg_data, len);
    return err;
}

static void
bmp388_delay_us(uint32_t period, void *intf_ptr)
{
    UNUSED_PARAMETER(intf_ptr);
    nrf_delay_us(period);
}

/* ++++++++++++++++++++++++++++++ Interface +++++++++++++++++++++++++++++++++ */
int8_t
bmp388_init(void)
{
    int8_t err = 0;
    uint8_t dev_addr = BMP3_ADDR_I2C_PRIM;

    dev.intf_ptr = &dev_addr;
    dev.intf = BMP3_I2C_INTF;
    dev.read = bmp388_twi_read;
    dev.write = bmp388_twi_write;
    dev.delay_us = bmp388_delay_us;

    /* Get chip id, do soft reset and read calibration coefficients */
    err = bmp3_init(&dev);
    if (err)
        return err;

    // Set sensor settings
    uint32_t settings_sel = 0;
    dev.settings.op_mode = BMP3_MODE_FORCED;
    dev.settings.press_en = 1;
    dev.settings.temp_en = 1;
    dev.settings.odr_filter.press_os = BMP3_OVERSAMPLING_16X;
    dev.settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
    dev.settings.odr_filter.iir_filter = BMP3_IIR_FILTER_COEFF_7;
    settings_sel = BMP3_SEL_PRESS_EN   |
                   BMP3_SEL_TEMP_EN    |
                   BMP3_SEL_PRESS_OS   |
                   BMP3_SEL_TEMP_OS    |
                   BMP3_SEL_IIR_FILTER;
    err = (uint32_t)bmp3_set_sensor_settings(settings_sel, &dev);
    if (err)
        return err;
    return err;
}

int8_t
bmp388_get_data(struct bmp3_data *comp_data)
{
    int8_t err = 0;
    err = bmp3_set_op_mode(&dev);
    if (err)
        return err;
    err = bmp3_get_sensor_data(BMP3_ALL, comp_data, &dev);
    if (err)
        return err;
    return err;
}
