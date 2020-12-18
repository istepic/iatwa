#include "sdcard.h"

NRF_BLOCK_DEV_SDC_DEFINE(
        m_block_dev_sdc,
        NRF_BLOCK_DEV_SDC_CONFIG(
                SDC_SECTOR_SIZE,
                APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)
         ),
         NFR_BLOCK_DEV_INFO_CONFIG("Nordic", "SDC", "1.00")
);

uint32_t
sd_write(char *sdata, size_t sdata_size)
{
    static FATFS fs;
    static DIR dir;
    static FILINFO fno;
    static FIL file;

    uint32_t bytes_written;
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    static diskio_blkdev_t drives[] =
    {
            DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        return disk_state;
    }

    // uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    // uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;

    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        return ff_result;
    }

    ff_result = f_opendir(&dir, "/");
    if (ff_result)
    {
        return ff_result;
    }

    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            return ff_result;
        }

        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                ;
            }
            else
            {
                ;
            }
        }
    }
    while (fno.fname[0]);

    ff_result = f_open(&file, FILE_NAME, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (ff_result != FR_OK)
    {
        return ff_result;
    }

    ff_result = f_write(&file, sdata, sdata_size, (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        return ff_result;
    }
    else
    {
        ;
    }

    (void) f_close(&file);
    return 0;
}
