#include "ms5607.h"

// MS5607 device address
#define MS5607_ADDR 0x76

// MS5607 device commands
#define MS5607_RESET_COMMAND 0x1E
#define MS5607_START_PRESSURE_ADC_CONVERSION 0x48
#define MS5607_START_TEMPERATURE_ADC_CONVERSION 0x58
#define MS5607_READ_ADC 0x00

// MS5607 Conversion time delay
#define MS5607_CONVERSION_TIME_OSR_4096 10 // miliseconds

#define MS5607_PROM_READ_FROM_START 0xA0

#define MS5607_CRC_INDEX 7
#define MS5607_PRESSURE_SENSITIVITY_INDEX 1
#define MS5607_PRESSURE_OFFSET_INDEX 2
#define MS5607_TEMP_COEFF_OF_PRESSURE_SENSITIVITY_INDEX 3
#define MS5607_TEMP_COEFF_OF_PRESSURE_OFFSET_INDEX 4
#define MS5607_REFERENCE_TEMPERATURE_INDEX 5
#define MS5607_TEMP_COEFF_OF_TEMPERATURE_INDEX 6
#define MS5607_COEFFICIENT_NUMBERS 8

static bool
ms5607_crc_check(uint16_t *n_prom, uint8_t crc)
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
ms5607_get_coeffs(uint16_t *coeffs)
{
    if (!coeffs)
        return (uint32_t)passed_null_pointer;
    uint32_t err = 0;
    uint8_t cmd = 0;
    uint8_t coeff_sample[2] = {0};

    for (size_t i = 0; i < 8; ++i)
    {
        /* each ROM register has to be accessed by sending 1010xyz0 where xyz
           are 3 bits representing 1 register, there are total of 8 registers */
        cmd = MS5607_PROM_READ_FROM_START + i * 2;
        err = twi_read(TWI_INS_1, MS5607_ADDR, (const uint8_t)cmd,
                 coeff_sample, sizeof(coeff_sample));
        if (err)
            return err;
        coeffs[i] = (coeff_sample[0] << 8) | coeff_sample[1];
    }
    if (!ms5607_crc_check(coeffs, coeffs[7] & 0x000F))
        return (uint32_t)crc_check_failed;
    return 0;
}

uint32_t
ms5607_get_data(int32_t *pres, int32_t *temp)
{
    if (!pres)
        return (uint32_t)passed_null_pointer;
    uint16_t coeffs[8] = {0};
    uint32_t err = 0;
    uint32_t pres_data = 0;
    uint32_t temp_data = 0;
    uint8_t data_sample[3] = {0};
    uint8_t cmd = 0;
    int64_t OFF, SENS, T2, OFF2, SENS2;
    int32_t dT, TEMP, P;

    err = ms5607_get_coeffs(coeffs);
    if (err)
        return err;

    cmd = MS5607_START_PRESSURE_ADC_CONVERSION;
    err = twi_tx(TWI_INS_1, MS5607_ADDR, &cmd, sizeof cmd);
    if (err)
        return err;
    nrf_delay_ms(10);

    cmd = MS5607_READ_ADC;
    twi_read(TWI_INS_1, MS5607_ADDR, (const uint8_t)cmd,
                 data_sample, sizeof(data_sample));
    pres_data = (data_sample[0] << 16) |
                (data_sample[1] << 8) |
                (data_sample[2]);

    cmd = MS5607_START_TEMPERATURE_ADC_CONVERSION;
    err = twi_tx(TWI_INS_1, MS5607_ADDR, &cmd, sizeof cmd);
    if (err)
        return err;
    nrf_delay_ms(10);

    cmd = MS5607_READ_ADC;
    twi_read(TWI_INS_1, MS5607_ADDR, (const uint8_t)cmd,
             data_sample, sizeof(data_sample));
    temp_data = (data_sample[0] << 16) |
                (data_sample[1] << 8) |
                (data_sample[2]);

    // Difference between actual and reference temperature = D2 - Tref
    dT = (int32_t)temp_data -
         ((int32_t)coeffs[MS5607_REFERENCE_TEMPERATURE_INDEX] << 8);
    // Actual temperature = 2000 + dT * TEMPSENS
    TEMP = 2000 + ((int64_t)dT *
           (int64_t)coeffs[MS5607_TEMP_COEFF_OF_TEMPERATURE_INDEX] >> 23);
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
    OFF = ((int64_t)(coeffs[MS5607_PRESSURE_OFFSET_INDEX]) << 16) +
          (((int64_t)(coeffs[MS5607_TEMP_COEFF_OF_PRESSURE_OFFSET_INDEX]) * dT) >> 7);
    OFF -= OFF2;

    // Sensitivity at actual temperature = SENS_T1 + TCS * dT
    SENS = ((int64_t)coeffs[MS5607_PRESSURE_SENSITIVITY_INDEX] << 15) +
           (((int64_t)coeffs[MS5607_TEMP_COEFF_OF_PRESSURE_SENSITIVITY_INDEX] * dT) >> 8);
    SENS -= SENS2;


    // Temperature compensated pressure = D1 * SENS - OFF
    P = (((pres_data * SENS) >> 21) - OFF) >> 15;
    printf("PRESS %ld\r\n", P);

    *pres = P;
    *temp = TEMP - T2;
    return 0;
}
