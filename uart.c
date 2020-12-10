#include "uart.h"

uint32_t
uart_init(void)
{
    uint32_t err;
    const app_uart_comm_params_t uart_conf = {
        .rx_pin_no = UART_RX_PIN,
        .tx_pin_no = UART_TX_PIN,
        .rts_pin_no = 0,
        .cts_pin_no = 0,
        .flow_control = 0,
        .use_parity = false,
        .baud_rate = NRF_UART_BAUDRATE_115200
    };

    APP_UART_FIFO_INIT(&uart_conf, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE,
                       NULL, APP_IRQ_PRIORITY_LOWEST,
                       err);

    return err;
}