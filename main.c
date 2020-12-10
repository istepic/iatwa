#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bsp.h"
#include "uart.h"
#include "sdcard.h"

#define SDATA_BUF_SIZE 512

uint32_t
sd_write_sensordata(struct sensor_data *sdata)
{
    char buf[SDATA_BUF_SIZE];
    memset(&buf, 0, sizeof buf);
    size_t sdata_size = snprintf(buf, SDATA_BUF_SIZE, "%s;%s\n",
                                 "1234",
                                 "5678");
    return sd_write(buf, sdata_size);
}

int main(void)
{
    ret_code_t err_code;

    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    if ((err_code = uart_init()) != NRF_SUCCESS) bsp_board_leds_on();
    if ((err_code = sd_write_sensordata(&sdata))) bsp_board_leds_on();

    printf("\r\nHello\r\n");
    for(;;)
    {
        ;
    }
    return 0;
}
