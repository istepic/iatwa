#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"

#include "bsp.h"
#include "uart.h"
#include "sdcard.h"
#include "my_errors.h"
#include "my_util.h"

#include "ms5611.h"
#include "bmp3.h"

#include "main_integration.h"

#define SENSORDATA_BUF_SIZE 512
#define NUM_OF_SENSORS 4

static enum {
    MS5611,
    BMP388,
} Sensors;

struct sensor_data {
    int32_t ms5611_pres;
    int32_t ms5611_temp;
    uint64_t bmp388_pres;
    int64_t bmp388_temp;
};

static uint32_t
get_sensordata(size_t sensor, struct sensor_data *sdata)
{
    uint32_t err = 0;
    switch(sensor)
    {
    case MS5611: ;
        int32_t p = 0;
        int32_t T = 0;
        err = ms5611_get_data(&p, &T);
        if (err)
            return err;
        sdata->ms5611_pres = p;
        sdata->ms5611_temp = T;
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
