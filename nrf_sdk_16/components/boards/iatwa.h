#ifndef IATWA_H
#define IATWA_H

// LED definition
#define LEDS_NUMBER    1
#define LED_1          10
#define LEDS_ACTIVE_STATE 0
#define LEDS_LIST { LED_1}
#define BSP_LED_0      LED_1
#define LEDS_INV_MASK  0

// BUTTON definition
#define BUTTONS_NUMBER 1
#define BUTTON_START   21
#define BUTTON_1       21
#define BUTTON_STOP    21
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
#define BUTTONS_ACTIVE_STATE 0
#define BUTTONS_LIST { BUTTON_1 }
#define BSP_BUTTON_0   BUTTON_1

// UART defintion
#define RX_PIN_NUMBER  28
#define TX_PIN_NUMBER  29
#define CTS_PIN_NUMBER UART_PIN_DISCONNECTED
#define RTS_PIN_NUMBER UART_PIN_DISCONNECTED
#define HWFC           false

// SPI defintion
#define SDC_SCK_PIN 7  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN 8 ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN 6 ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN 9   ///< SDC chip select (CS) pin.

// TWI defintion
#define TWI_0_SDA_PIN 12
#define TWI_0_SCL_PIN 14
#define TWI_1_SDA_PIN 19
#define TWI_1_SCL_PIN 17

#endif // IATWA_H
