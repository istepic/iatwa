#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "uart.h"
#include "bsp.h"

int main(void)
{
    uint32_t err_code;
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    err_code = uart_init();
    if (err_code == NRF_SUCCESS)
        bsp_board_leds_on();
    else
        bsp_board_leds_off();

    printf("\r\nHello\r\n");
    for(;;)
    {
        uint8_t ch = 0;
        while (app_uart_get(&ch) != NRF_SUCCESS);
        while (app_uart_put(ch) != NRF_SUCCESS);
        if (ch == 't')
        {
            bsp_board_leds_off();
            while(1)
            {
                ;
            }
        }
    }
    return 0;
}
