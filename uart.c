#include "uart.h"

static void
uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        bsp_board_leds_on();
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        bsp_board_leds_on();
    }
}

uint32_t
uart_init(void)
{
    uint32_t err;
    const app_uart_comm_params_t uart_conf = {
        .rx_pin_no = RX_PIN_NUMBER,
        .tx_pin_no = TX_PIN_NUMBER,
        .rts_pin_no = 0,
        .cts_pin_no = 0,
        .flow_control = 0,
        .use_parity = false,
        .baud_rate = NRF_UART_BAUDRATE_115200
    };

    APP_UART_FIFO_INIT(&uart_conf, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE,
                       uart_error_handle, APP_IRQ_PRIORITY_LOWEST,
                       err);

    return err;
}