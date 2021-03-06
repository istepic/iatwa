cmake_minimum_required(VERSION 3.14)
project(iatwa LANGUAGES C ASM)

list(APPEND CMAKE_MODULE_PATH "cmake")
include("nrf5")

add_executable(iatwa "main.c" "uart.c" "sdcard.c" "ms5607.c" "bmp3.c" "bmp_integration.c" "twi.c" "my_util.c" "dps368.c" "smpb.c")
nrf5_target(iatwa)
nrf5_target_link_libraries(iatwa PRIVATE
    # basic stuff
    nrf5_bsp
    nrf5_app_uart_fifo   #redirect is added into this module, should be separate
    # sdcard stuff
    nrf5_nrfx_spim
    nrf5_ext_fatfs
    nrf5_ext_fatfs_port_diskio_blkdev
    nrf5_block_dev
    nrf5_block_dev_sdc
    nrf5_app_sdcard
    # twi stuff
    nrf5_drv_twi
    # clock
    nrf5_drv_clock
    nrf5_app_timer
    nrf5_app_scheduler
    # watchdog
    nrf5_nrfx_wdt
 )

 target_link_libraries(iatwa PRIVATE m)
