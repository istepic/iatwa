#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "nrf_delay.h"

#include "bsp.h"
#include "uart.h"
#include "sdcard.h"
#include "my_errors.h"
#include "my_util.h"

// #include "ms5607.h"
#include "bmp3.h"
#include "bmp_integration.h"
#include "dps368.h"
#include "smpb.h"

#define SENSORDATA_BUF_SIZE 512
#define NUM_OF_SENSORS 3
#define CONFIG_NFCT_PINS_AS_GPIOS

enum {
//    MS5607,
    BMP388,
    DPS368,
    SMPB,
};

struct sensor_data {
    int32_t ms5607_pres;
    int32_t ms5607_temp;
    double bmp388_pres;
    double bmp388_temp;
    double dps368_pres;
    double dps368_temp;
    double smpb_pres;
    double smpb_temp;
};

static void
enable_nfc_pins_as_gpio(void)
{
    #if defined (CONFIG_NFCT_PINS_AS_GPIOS)
        if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) == (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)){
            NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
            NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
            NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
            NVIC_SystemReset();
        }
    #endif
}

static uint32_t
get_sensordata(size_t sensor, struct sensor_data *sdata)
{
    uint32_t err = 0;
    switch(sensor)
    {
    // case MS5607: break;
    //    int32_t ms5607_p = 0;
    //    int32_t ms5607_T = 0;
    //    err = ms5607_get_data(&ms5607_p, &ms5607_T);
    //    if (err)
    //        return err;
    //    sdata->ms5607_pres = ms5607_p;
    //    sdata->ms5607_temp = ms5607_T;
    //    break;
    case BMP388: ;
       struct bmp3_data bdata;
       memset(&bdata, 0, sizeof(struct bmp3_data));
       err = (uint32_t)bmp388_get_data(&bdata);
       if (err)
           return err;
       sdata->bmp388_pres = (double)bdata.pressure / 100;
       sdata->bmp388_temp = (double)bdata.temperature / 100;
       printf("BMP388 Pressure: %f Temperature: %f\r\n", sdata->bmp388_pres, sdata->bmp388_temp);
       break;
     case DPS368: ;
        double dps368_p = 0;
        double dps368_T = 0;
        err = dps368_get_data(&dps368_p, &dps368_T);
        if (err)
            return err;
        sdata->dps368_pres = dps368_p;
        sdata->dps368_temp = dps368_T;
        printf("DPS368 Pressure: %f Temperature: %f\r\n", dps368_p, dps368_T);
        break;
    case SMPB: ;
        double smpb_p = 0;
        double smpb_T = 0;
        err = smpb_get_data(&smpb_p, &smpb_T);
        if (err)
            return err;
        sdata->smpb_pres = smpb_p;
        sdata->smpb_temp = smpb_T;
        printf("2SMPB Pressure: %f Temperature:%f\r\n", smpb_p, smpb_T);
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
    char buf[64] = {0};
    for (size_t i = 0; i < NUM_OF_SENSORS; ++i)
    {
        snprintf(buf, sizeof buf, "%.2f,%.3f;%.2f,%.3f;%.2f,%.3f\r\n",
                 sdata->bmp388_temp, sdata->bmp388_pres,
                 sdata->dps368_temp, sdata->dps368_pres,
                 sdata->smpb_temp, sdata->smpb_pres);
    }
    return sd_write(buf, strlen(buf));
}

int main(void)
{
    uint32_t err = 0;
    struct sensor_data sdata;
    memset(&sdata, 0, sizeof(struct sensor_data));
    enable_nfc_pins_as_gpio();
    nrf_delay_ms(40); // Some sensors need time to initalize

    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    bsp_board_leds_on();
    if ((err = uart_init()) != NRF_SUCCESS)
        printf("UART Error: %lu\r\n", err);
        bsp_board_leds_on();
    printf("### Application started ###\r\n");
    if ((err = twi_init()) != NRF_SUCCESS)
        printf("TWI Error: %lu\r\n", err);
        bsp_board_leds_on();
    printf("TWI Initialized\r\n");
    if((err = (uint32_t)bmp388_init()))
        printf("BMP388 Error: %lu\r\n", err);
       bsp_board_leds_on();
    printf("BMP388 Initialized\r\n");
    if((err = dps368_init()))
        printf("DPS368 Error: %lu\r\n", err);
        bsp_board_leds_on();
    printf("DPS368 Initialized\r\n");
    if((err = smpb_init()))
        printf("2SMPB Error: %lu\r\n", err);
        bsp_board_leds_on();
    printf("2SMPB-02E Initialized\r\n");

    sd_write("\nMEASUREMENT\n", strlen("\nMEASUREMENT\n"));

    while(1)
    {
        if ((err = get_data(&sdata)))
        {
            printf("Something went wrong with fetching the sensor data ERROR %lu\r\n", err);
            bsp_board_leds_on();
            break;
        }
        nrf_delay_ms(2000);
        // if ((err = sd_write_sensordata(&sdata)))
        // {
        //     printf("Failed to write data to SD Card %lu\r\n", err);
        //     bsp_board_leds_on();
        //     break;
        // }
    }
    return 0;
}
