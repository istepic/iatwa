#ifndef SDCARD_H
#define SDCARD_H

#include "nrf_block_dev_sdc.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "boards.h"

uint32_t sd_write(char *, size_t);

#define FILE_NAME "data.txt"

#endif
