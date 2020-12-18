#include "ms5611.h"

extern union Sample sample;

// MS5611 device address
#define MS5611_ADDR 0x76

// MS5611 device commands
#define MS5611_RESET_COMMAND 0x1E
#define MS5611_START_PRESSURE_ADC_CONVERSION 0x48
#define MS5611_START_TEMPERATURE_ADC_CONVERSION 0x58
#define MS5611_READ_ADC 0x00

// MS5611 Conversion time delay
#define MS5611_CONVERSION_TIME_OSR_4096 10 // miliseconds

#define MS5611_PROM_READ_FROM_START 0xA0

#define MS5611_CRC_INDEX 7
#define MS5611_PRESSURE_SENSITIVITY_INDEX 1
#define MS5611_PRESSURE_OFFSET_INDEX 2
#define MS5611_TEMP_COEFF_OF_PRESSURE_SENSITIVITY_INDEX 3
#define MS5611_TEMP_COEFF_OF_PRESSURE_OFFSET_INDEX 4
#define MS5611_REFERENCE_TEMPERATURE_INDEX 5
#define MS5611_TEMP_COEFF_OF_TEMPERATURE_INDEX 6
#define MS5611_COEFFICIENT_NUMBERS 8

static bool
ms5611_crc_check(uint16_t *n_prom, uint8_t crc)
{
    if (!n_prom)
        return false;
    uint8_t cnt, n_bit;
    uint16_t n_rem;
    uint16_t crc_read;

    n_rem = 0x00;
    crc_read = n_prom[7];
    n_prom[7] = (0xFF00 & (n_prom[7]));
    for (cnt = 0; cnt < 16; cnt++)
    {
        if (cnt % 2 == 1)
            n_rem ^= (unsigned short)((n_prom[cnt >> 1]) & 0x00FF);
        else
            n_rem ^= (unsigned short)(n_prom[cnt >> 1] >> 8);
        for (n_bit = 8; n_bit > 0; n_bit--)
        {
            if (n_rem & (0x8000))
                n_rem = (n_rem << 1) ^ 0x3000;
            else
                n_rem = (n_rem << 1);
        }
    }
    n_rem = (0x000F & (n_rem >> 12));
    n_prom[7] = crc_read;
    n_rem ^= 0x00;

    return (n_rem == crc);
}

static uint32_t
ms5611_get_coeffs(uint16_t *coeffs)
{
    if (!coeffs)
        return (uint32_t)passed_null_pointer;
    uint32_t err = 0;
    uint8_t cmd = 0;
    sample.two_bytes[0] = 0;
    sample.two_bytes[1] = 0;

    for (size_t i = 0; i < 8; ++i)
    {
        cmd = MS5611_PROM_READ_FROM_START + i * 2;
        /* each ROM register has to be accessed by sending 1010xyz0 where xyz
           are 3 bits representing 1 register, there are total of 8 registers */
        err = twi_tx(0, MS5611_ADDR, &(cmd), sizeof cmd);
        if (err)
            return err;
        /* rx sample via global variable "sample" */
        err = twi_rx(0, MS5611_ADDR);
        if (err)
            return err;
        coeffs[i] = (sample.two_bytes[0] << 8) | sample.two_bytes[1];
    }
    if (!ms5611_crc_check(coeffs, coeffs[7] & 0x000F))
        return (uint32_t)crc_check_failed;
    return 0;
}

uint32_t ms5611_get_data(float *pres, float *temp)
{
    if (!pres)
        return (uint32_t)passed_null_pointer;
    uint16_t coeffs[8] = {0};
    uint32_t err = 0;
    uint32_t pres_data = 0;
    uint32_t temp_data = 0;
    sample.three_bytes[0] = 0;
    sample.three_bytes[1] = 0;
    sample.three_bytes[2] = 0;
    uint8_t cmd_start_pres = MS5611_START_PRESSURE_ADC_CONVERSION;
    uint8_t cmd_start_temp = MS5611_START_TEMPERATURE_ADC_CONVERSION;
    uint8_t cmd_read_adc = MS5611_READ_ADC;
    int64_t OFF, SENS, T2, OFF2, SENS2;
    int32_t dT, TEMP, P;

    err = ms5611_get_coeffs(coeffs);
    if (err)
        return err;
    err = twi_tx(0, MS5611_ADDR, &cmd_start_pres, sizeof cmd_start_pres);
    if (err)
        return err;
    nrf_delay_ms(10);
    err = twi_tx(0, MS5611_ADDR, &cmd_read_adc, sizeof cmd_read_adc);
    if (err)
        return err;
    err = twi_rx(0, MS5611_ADDR);
    if (err)
        return err;
    pres_data = (sample.three_bytes[0] << 16) |
                (sample.three_bytes[1] << 8) |
                (sample.three_bytes[2]);

    sample.three_bytes[0] = 0;
    sample.three_bytes[1] = 0;
    sample.three_bytes[2] = 0;
    err = twi_tx(0, MS5611_ADDR, &cmd_start_temp, sizeof cmd_start_temp);
    if (err)
        return err;
    nrf_delay_ms(10);
    err = twi_tx(0, MS5611_ADDR, &cmd_read_adc, sizeof cmd_read_adc);
    if (err)
        return err;
    err = twi_rx(0, MS5611_ADDR);
    if (err)
        return err;
    temp_data = (sample.three_bytes[0] << 16) |
                (sample.three_bytes[1] << 8) |
                (sample.three_bytes[2]);

    // Difference between actual and reference temperature = D2 - Tref
    dT = (int32_t)temp_data - ((int32_t)coeffs[MS5611_REFERENCE_TEMPERATURE_INDEX] << 8);
    // Actual temperature = 2000 + dT * TEMPSENS
    TEMP = 2000 + ((int64_t)dT * (int64_t)coeffs[MS5611_TEMP_COEFF_OF_TEMPERATURE_INDEX] >> 23);
    printf("TEMP %ld\r\n", TEMP);
    // Second order temperature compensation
    if (TEMP < 2000)
    {
        T2 = (3 * ((int64_t)dT * (int64_t)dT)) >> 33;
        OFF2 = 61 * ((int64_t)TEMP - 2000) * ((int64_t)TEMP - 2000) / 16;
        SENS2 = 29 * ((int64_t)TEMP - 2000) * ((int64_t)TEMP - 2000) / 16;

        if (TEMP < -1500)
        {
            OFF2 += 17 * ((int64_t)TEMP + 1500) * ((int64_t)TEMP + 1500);
            SENS2 += 9 * ((int64_t)TEMP + 1500) * ((int64_t)TEMP + 1500);
        }
    }
    else
    {
        T2 = 0;
        OFF2 = 0;
        SENS2 = 0;
    }

    // OFF = OFF_T1 + TCO * dT
    OFF = ((int64_t)(coeffs[MS5611_PRESSURE_OFFSET_INDEX]) << 16) + (((int64_t)(coeffs[MS5611_TEMP_COEFF_OF_PRESSURE_OFFSET_INDEX]) * dT) >> 7);
    OFF -= OFF2;

    // Sensitivity at actual temperature = SENS_T1 + TCS * dT
    SENS = ((int64_t)coeffs[MS5611_PRESSURE_SENSITIVITY_INDEX] << 15) + (((int64_t)coeffs[MS5611_TEMP_COEFF_OF_PRESSURE_SENSITIVITY_INDEX] * dT) >> 8);
    SENS -= SENS2;


    // Temperature compensated pressure = D1 * SENS - OFF
    P = (((pres_data * SENS) >> 21) - OFF) >> 15;
    printf("PRESS %ld\r\n", P);

    *pres = (float)P / 100;
    *temp = ((float)TEMP - T2) / 100;
    return 0;
}
