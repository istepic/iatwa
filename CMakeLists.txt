cmake_minimum_required(VERSION 3.14)
project(diplomski_rad LANGUAGES C ASM)

list(APPEND CMAKE_MODULE_PATH "cmake")
include("nrf5")

add_executable(diplomski_rad "main.c" "uart.c" "sdcard.c" "ms5611.c" "twi.c" "my_util.c")
nrf5_target(diplomski_rad)
nrf5_target_link_libraries(diplomski_rad PRIVATE
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
 )
