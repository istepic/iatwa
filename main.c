#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"

#include "bsp.h"
#include "uart.h"
#include "sdcard.h"
#include "my_errors.h"
#include "my_util.h"

#include "ms5607.h"
#include "bmp3.h"
#include "bmp_integration.h"
#include "dps368.h"
#include "smpb.h"

#define SENSORDATA_BUF_SIZE 512
#define NUM_OF_SENSORS 4

enum {
    MS5607,
    BMP388,
    DPS368,
    SMPB,
};

struct sensor_data {
    int32_t ms5607_pres;
    int32_t ms5607_temp;
    uint64_t bmp388_pres;
    int64_t bmp388_temp;
    int32_t dps368_pres;
    int32_t dps368_temp;
    double smpb_pres;
    double smpb_temp;
};

static uint32_t
get_sensordata(size_t sensor, struct sensor_data *sdata)
{
    uint32_t err = 0;
    switch(sensor)
    {
    case MS5607: ;
        int32_t ms5607_p = 0;
        int32_t ms5607_T = 0;
        err = ms5607_get_data(&ms5607_p, &ms5607_T);
        if (err)
            return err;
        sdata->ms5607_pres = ms5607_p;
        sdata->ms5607_temp = ms5607_T;
        break;
    case BMP388: ;
        struct bmp3_data bdata;
        memset(&bdata, 0, sizeof(struct bmp3_data));
        err = (uint32_t)bmp388_get_data(&bdata);
        if (err)
            return err;
        sdata->bmp388_pres = bdata.pressure;
        sdata->bmp388_temp = bdata.temperature;
        break;
    case DPS368: ;
        int32_t dps368_p = 0;
        int32_t dps368_T = 0;
        err = dps368_get_data(&dps368_p, &dps368_T);
        if (err)
            return err;
        sdata->dps368_pres = dps368_p;
        sdata->dps368_temp = dps368_T;
        break;
    case SMPB: ;
        double smpb_p = 0;
        double smpb_T = 0;
        err = smpb_get_data(&smpb_p, &smpb_T);
        if (err)
            return err;
        sdata->smpb_pres = smpb_p;
        sdata->smpb_temp = smpb_T;
        break;
    default:
        return reached_default;
    }
    return err;
}

static uint32_t
get_data(struct sensor_data *sdata)
{
    uint32_t err = 0;
    for (size_t i = 0; i < NUM_OF_SENSORS; ++i)
    {
        err = get_sensordata(i, sdata);
        if (err)
            return err;
    }
    return err;
}

static uint32_t
sd_write_sensordata(struct sensor_data *sdata)
{
    char data[33]; //remove this and refactor this function for new structure
    char buf[64];
    memset(&buf, 0, sizeof buf);
    char sensor_data[512];
    memset(&sensor_data, 0, sizeof sensor_data);
    char *ptr = sensor_data;
    for (size_t i = 0; i < NUM_OF_SENSORS; ++i)
    {
        float_to_string(data[i], buf, 2);
        size_t buf_size = strlen(buf);
        memcpy(ptr, buf, buf_size);
        ptr = ptr + buf_size + 1;
        *ptr = ';';
        ptr = ptr + 1;
    }
    return sd_write(sensor_data, strlen(sensor_data));
}

int main(void)
{
    uint32_t err = 0;
    struct sensor_data sdata;

    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    if ((err = uart_init()) != NRF_SUCCESS)
        bsp_board_leds_on();
    printf("### Application started ###\r\n");
    if ((err = twi_init()) != NRF_SUCCESS)
        bsp_board_leds_on();
    printf("TWI Initialized\r\n");
    if((err = (uint32_t)bmp388_init()))
        bsp_board_leds_on();
    printf("BMP388 Initialized\r\n");
    if((err = dps368_init()))
        bsp_board_leds_on();
    printf("DPS368 Initialized\r\n");
    if((err = smpb_init()))
        bsp_board_leds_on();
    printf("2SMPB-02E Initialized\r\n");

    while(1)
    {
        if ((err = get_data(&sdata)))
        {
            bsp_board_leds_on();
            break;
        }
        if ((err = sd_write_sensordata(&sdata)))
        {
            bsp_board_leds_on();
            break;
        }
    }
    return 0;
}
