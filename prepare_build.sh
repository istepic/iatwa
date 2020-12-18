#!/bin/bash

cmake                                                     \
   -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake       \
   -DTOOLCHAIN_PREFIX=/home/istep/gcc-arm-none-eabi-9-2020-q2-update \
   -DNRF5_SDK_PATH=~/iatwa_DK/nrf_sdk_16          \
   -DNRF5_BOARD=pca10040                                  \
   -DNRF5_SOFTDEVICE_VARIANT=s132                         \
   -DNRF5_SDKCONFIG_PATH=config                           \
   -DNRF5_LINKER_SCRIPT=config/iatwa.ld                   \
   -G "Unix Makefiles"                                    \
   -B build                                               \
   -S .
