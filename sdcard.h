#ifndef SDCARD_H
#define SDCARD_H

#include "nrf_block_dev_sdc.h"
#include "ff.h"
#include "diskio_blkdev.h"

uint32_t sd_write(char *, size_t);

#define FILE_NAME "data.txt"

#define SDC_SCK_PIN 22  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN 23 ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN 24 ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN 20   ///< SDC chip select (CS) pin.

#endif