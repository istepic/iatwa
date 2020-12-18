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

#define SENSORDATA_BUF_SIZE 512
#define NUM_OF_DIFF_SENSOR_DATA 8

uint32_t (*sensordata_getters[1])(float *, float *) = {
    ms5611_get_data,
};

static uint32_t
get_sensordata(float *data)
{
    uint32_t err = 0;
    for (size_t i = 0; i < 1; i = i + 2)
    {
        err = sensordata_getters[i](&data[i], &data[i+1]);
        if (err)
            return err;
    }
    return err;
}

static uint32_t
sd_write_sensordata(float *data)
{
    char buf[64];
    memset(&buf, 0, sizeof buf);
    char sensor_data[512];
    memset(&sensor_data, 0, sizeof sensor_data);
    char *ptr = sensor_data;
    for (size_t i = 0; i < NUM_OF_DIFF_SENSOR_DATA; ++i)
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
    float sensor_data[NUM_OF_DIFF_SENSOR_DATA] = {0};

    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    if ((err = uart_init()) != NRF_SUCCESS)
        bsp_board_leds_on();
    printf("### Application started ###\r\n");
    if ((err = twi_init()) != NRF_SUCCESS)
        bsp_board_leds_on();
    printf("TWI Initialized\r\n");

    while(1)
    {
        if ((err = get_sensordata(sensor_data)))
        {
            bsp_board_leds_on();
            break;
        }
        if ((err = sd_write_sensordata(sensor_data)))
        {
            bsp_board_leds_on();
            break;
        }
    }
    return 0;
}
