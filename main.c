#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "app_timer.h"
#include "app_scheduler.h"

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

#define NUM_OF_SENSORS 3
#define MOLAR_MASS_OF_AIR 0.0289647 // kg/mol
#define GAS_CONSTANT 8.3144626 // kg*m2/K*mol*s2
#define GRAVITY 9.80665 // m/s2

#define CONFIG_NFCT_PINS_AS_GPIOS
#define CONFIG_GPIO_AS_PINRESET

// Scheduler settings
#define SCHED_MAX_EVENT_DATA_SIZE   APP_TIMER_SCHED_EVENT_DATA_SIZE
#define SCHED_QUEUE_SIZE            10

// App timer settings
#define TIME_INTERVAL APP_TIMER_TICKS(450)
APP_TIMER_DEF(m_app_timer_id);

struct sensor_data {
    // int32_t ms5607_pres;
    // int32_t ms5607_temp;
    double bmp388_pres;
    double bmp388_temp;
    double dps368_pres;
    double dps368_temp;
    double smpb_pres;
    double smpb_temp;
};

/* STATIC FUNCTIONS DECLARATION */
static void clock_init(void);
static void app_timer_handler(void *);
static void scheduler_event_handler(void *, uint16_t);
static void timer_init(void);
static void enable_nfc_pins_as_gpio(void);
static void enable_reset_pin(void);
static uint32_t get_sensordata(size_t, struct sensor_data *);
static uint32_t get_data(struct sensor_data *);
static uint32_t sd_write_sensordata(struct sensor_data *);
static void power_manage(void);

enum {
//    MS5607,
    BMP388,
    DPS368,
    SMPB,
};

/* Clock initialization */
static void
clock_init(void)
{
    uint32_t err = 0;
    err = nrf_drv_clock_init();
    if (err)
    {
        printf("Failed to initialize clock drivers: %lu\r\n", err);
        return;
    }
    nrf_drv_clock_lfclk_request(NULL);
}

/* Schedueled function to be ran. This is where reading the sensor data and
   writing to SD card happens */
static void
scheduler_event_handler(void *p_event_data, uint16_t event_size)
{
    uint32_t err = 0;
    bsp_board_leds_off();
    struct sensor_data sdata;
    memset(&sdata, 0, sizeof(struct sensor_data));
    if ((err = get_data(&sdata)))
    {
        printf("Something went wrong with fetching the sensor data ERROR %lu\r\n", err);
        bsp_board_leds_on();
    }
    if ((err = sd_write_sensordata(&sdata)))
    {
        printf("Failed to write data to SD Card %lu\r\n", err);
        bsp_board_leds_on();
    }
}

/* This is interrupt handler for App timer. This is triggered every 100ms and
all it does is it puts scheduler_event_handler to queue that is then ran in
thread mode instead of interrupt mode. */
static void
app_timer_handler(void *p_context)
{
    app_sched_event_put(NULL, 0, scheduler_event_handler);
}

/* Timer initialization */
static void
timer_init(void)
{
    uint32_t err = 0;
    err = app_timer_init();
    if (err)
    {
        printf("App Timer initialization failed: %lu\r\n", err);
        return;
    }
    err = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED,
                           app_timer_handler);
    if (err)
    {
        printf("App Timer creation failed: %lu\r\n", err);
        return;
    }
}

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

static void
enable_reset_pin(void)
{
    #if defined (CONFIG_GPIO_AS_PINRESET)
    if (((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos)) ||
        ((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos))){
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_UICR->PSELRESET[0] = 21;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_UICR->PSELRESET[1] = 21;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NVIC_SystemReset();
    }
#endif
}

/* Reads data from pressure sensors */
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
        printf("2SMPB Pressure: %f Temperature: %f\r\n", smpb_p, smpb_T);
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

/* Write data so SD card */
static uint32_t
sd_write_sensordata(struct sensor_data *sdata)
{
    char buf[64] = {0};
    double bmp388_altitude = - (double)45076.9231 * (pow(sdata->bmp388_pres / (double)101325, -(((double)GAS_CONSTANT * (double)(-0.0065)) / ((double)GRAVITY * (double)MOLAR_MASS_OF_AIR))) - 1);
    double dps368_altitude = - (double)45076.9231 * (pow(sdata->dps368_pres / (double)101325, -(((double)GAS_CONSTANT * (double)(-0.0065)) / ((double)GRAVITY * (double)MOLAR_MASS_OF_AIR))) - 1);
    double smpb_altitude = - (double)45076.9231 * (pow(sdata->smpb_pres / (double)101325, -(((double)GAS_CONSTANT * (double)(-0.0065)) / ((double)GRAVITY * (double)MOLAR_MASS_OF_AIR))) - 1);
    // double bmp388_altitude = -((sdata->bmp388_temp * (double)GAS_CONSTANT) / ((double)MOLAR_MASS_OF_AIR * (double)GRAVITY)) * log(sdata->bmp388_pres / (double)101325);
    // double dps368_altitude = -((sdata->dps368_temp * (double)GAS_CONSTANT) / ((double)MOLAR_MASS_OF_AIR * (double)GRAVITY)) * log(sdata->dps368_pres / (double)101325);
    // double smpb_altitude = -((sdata->smpb_temp * (double)GAS_CONSTANT) / ((double)MOLAR_MASS_OF_AIR * (double)GRAVITY)) * log(sdata->smpb_pres / (double)101325);
    printf("BMP388:%.4f   DPS368:%.4f   2SMPB:%.4f\r\n", bmp388_altitude, dps368_altitude, smpb_altitude);
    for (size_t i = 0; i < NUM_OF_SENSORS; ++i)
    {
        // snprintf(buf, sizeof buf, "%.2f,%.3f;%.2f,%.3f;%.2f,%.3f\r\n",
        //          sdata->bmp388_temp, sdata->bmp388_pres,
        //          sdata->dps368_temp, sdata->dps368_pres,
        //          sdata->smpb_temp, sdata->smpb_pres);
        snprintf(buf, sizeof buf, "%.2f;%.2f;%.2f\r\n",
                 bmp388_altitude,
                 dps368_altitude,
                 smpb_altitude);
    }
    return sd_write(buf, strlen(buf));
}

static void power_manage(void)
{
    __WFI();
}

int main(void)
{
    enable_nfc_pins_as_gpio();
    enable_reset_pin();
    uint32_t err = 0;
    bsp_board_init(BSP_INIT_LEDS);
    bsp_board_leds_off();

    if ((err = uart_init()) != NRF_SUCCESS)
    {
        printf("UART Error: %lu\r\n", err);
        bsp_board_leds_on();
        while(1);
    }
    printf("### Application started ###\r\n");
    if ((err = twi_init()) != NRF_SUCCESS)
    {
        printf("TWI Error: %lu\r\n", err);
        bsp_board_leds_on();
        while(1);
    }
    printf("TWI Initialized\r\n");
    if((err = (uint32_t)bmp388_init()))
    {
        printf("BMP388 Error: %lu\r\n", err);
        bsp_board_leds_on();
        while(1);
    }
    printf("BMP388 Initialized\r\n");
    if((err = dps368_init()))
    {
        printf("DPS368 Error: %lu\r\n", err);
        bsp_board_leds_on();
        while(1);
    }
    printf("DPS368 Initialized\r\n");
    if((err = smpb_init()))
    {
        printf("2SMPB Error: %lu\r\n", err);
        bsp_board_leds_on();
        while(1);
    }
    printf("2SMPB-02E Initialized\r\n");

    // Every time we reset, append "MEASUREMENT" to file that contains sensor data
    sd_write("MEASUREMENT\n", strlen("MEASUREMENT\n"));

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    clock_init();
    timer_init();
    if ((err = app_timer_start(m_app_timer_id, TIME_INTERVAL, NULL)))
    {
        printf("App Timer failed to start: %lu\r\n", err);
        bsp_board_leds_on();
        while(1);
    }

    while(1)
    {
        app_sched_execute();
        power_manage();
    }
    return 0;
}
