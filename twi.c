#include "twi.h"

static const nrf_drv_twi_t m_twi_0 = NRF_DRV_TWI_INSTANCE(0);
static const nrf_drv_twi_t m_twi_1 = NRF_DRV_TWI_INSTANCE(1);
static volatile bool twi_transfer_done;

static void
twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context)
{
    switch (p_event->type)
    {
    case NRF_DRV_TWI_EVT_DONE:
        twi_transfer_done = true;
        break;
    default:
        break;
    }
}

static uint32_t
twi0_init(void)
{
    uint32_t err = 0;
    const nrf_drv_twi_config_t twi0_config = {
        .scl = TWI_0_SCL_PIN,
        .sda = TWI_0_SDA_PIN,
        .frequency = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = true};

    err = nrf_drv_twi_init(&m_twi_0, &twi0_config, twi_handler, NULL);
    if (err)
        return err;
    nrf_drv_twi_enable(&m_twi_0);
    return 0;
}

static uint32_t
twi1_init(void)
{
    uint32_t err = 0;
    const nrf_drv_twi_config_t twi1_config = {
        .scl = TWI_1_SCL_PIN,
        .sda = TWI_1_SDA_PIN,
        .frequency = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = true};

    err = nrf_drv_twi_init(&m_twi_1, &twi1_config, twi_handler, NULL);
    if (err)
        return err;
    nrf_drv_twi_enable(&m_twi_1);
    return 0;
}

/* ++++++++++++++++++++++++++++++ Interface ++++++++++++++++++++++++++++++++++*/

/**
 * @brief TWI Init
 */
uint32_t
twi_init(void)
{
    uint32_t err = 0;
    if ((err = twi0_init()) || (err = twi1_init()))
        return err;
    return 0;
}

/**
 * @brief TWI RX
 * @param twi_ins TWI instance
 * @param addr I2C address
 * @param data Data pointer
 * @param size Data size
 */
uint32_t
twi_rx(const uint8_t twi_ins, const uint8_t twi_addr, uint8_t *data,
       size_t size)
{
    if (!data)
        return (uint32_t)passed_null_pointer;
    twi_transfer_done = false;
    uint32_t err = 0;
    switch (twi_ins)
    {
    case 0:
        err = nrf_drv_twi_rx(&m_twi_0, twi_addr, data, size);
        while (twi_transfer_done == false)
            ;
    case 1:
        err = nrf_drv_twi_rx(&m_twi_1, twi_addr, data, size);
        while (twi_transfer_done == false)
            ;
    }
    return err;
}

/**
 * @brief TWI TX
 * @param twi_ins TWI instance
 * @param addr I2C address
 * @param data Data pointer
 * @param size Data size
 */
uint32_t
twi_tx(const uint8_t twi_ins, const uint8_t twi_addr, uint8_t *data,
       size_t size)
{
    if (!data)
        return (uint32_t)passed_null_pointer;
    twi_transfer_done = false;
    uint32_t err = 0;
    switch (twi_ins)
    {
    case 0:
        err = nrf_drv_twi_tx(&m_twi_0, twi_addr, data, size, false);
        while (twi_transfer_done == false)
            ;
    case 1:
        err = nrf_drv_twi_tx(&m_twi_1, twi_addr, data, size, false);
        while (twi_transfer_done == false)
            ;
    }
    return err;
}

/**
 * @brief TWI Read, involves sending command with tx and then reading with rx
 * @param twi_ins TWI instance
 * @param addr I2C address
 * @param data Data pointer
 * @param size Data size
 */
uint32_t
twi_read(const uint8_t twi_ins, const uint8_t twi_addr, uint8_t *cmd,
         size_t cmd_size, uint8_t *data, size_t data_size)
{
    if (!data || !cmd)
        return passed_null_pointer;
    twi_transfer_done = false;
    uint32_t err = 0;
    err = twi_tx(twi_ins, twi_addr, cmd, cmd_size);
    if (err)
        return err;
    err = twi_rx(twi_ins, twi_addr, data, data_size);
    if (err)
        return err;
    return err;
}

