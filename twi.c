#include "twi.h"

static const nrf_drv_twi_t m_twi_0 = NRF_DRV_TWI_INSTANCE(0);
static const nrf_drv_twi_t m_twi_1 = NRF_DRV_TWI_INSTANCE(1);
static volatile bool twi_transfer_done;

union Sample sample;

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

uint32_t
twi_init(void)
{
    uint32_t err = 0;
    if ((err = twi0_init()) || (err = twi1_init()))
        return err;
    return 0;
}

uint32_t
twi_rx(const uint8_t twi_ins, const uint8_t addr)
{
    twi_transfer_done = false;
    uint32_t err = 0;
    void *sample_ptr = &sample;
    switch (twi_ins)
    {
    case 0:
        err = nrf_drv_twi_rx(&m_twi_0, addr, sample_ptr, sizeof(sample));
        while (twi_transfer_done == false)
            ;
    case 1:
        err = nrf_drv_twi_rx(&m_twi_1, addr, sample_ptr, sizeof(sample));
        while (twi_transfer_done == false)
            ;
    }
    return err;
}

uint32_t
twi_tx(const uint8_t twi_ins, const uint8_t addr, uint8_t *data, uint8_t size)
{
    if (!data)
        return (uint32_t)passed_null_pointer;
    twi_transfer_done = false;
    uint32_t err = 0;
    switch (twi_ins)
    {
    case 0:
        err = nrf_drv_twi_tx(&m_twi_0, addr, data, size, false);
        while (twi_transfer_done == false)
            ;
    case 1:
        err = nrf_drv_twi_tx(&m_twi_1, addr, data, size, false);
        while (twi_transfer_done == false)
            ;
    }
    return err;
}
