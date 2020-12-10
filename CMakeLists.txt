cmake_minimum_required(VERSION 3.14)
project(diplomski_rad LANGUAGES C ASM)

list(APPEND CMAKE_MODULE_PATH "cmake")
include("nrf5")

add_executable(diplomski_rad "main.c" "uart.c")
nrf5_target(diplomski_rad)
nrf5_target_link_libraries(diplomski_rad PRIVATE
    nrf5_bsp
    nrf5_app_uart_fifo   #redirect is added into this module, should be separate
 )
