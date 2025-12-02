#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

set(SDK_ROOT_PATH   ${CMAKE_CURRENT_SOURCE_DIR}/../..)
list(APPEND BOARD_ROOT ${SDK_ROOT_PATH}) 
set(SNIPPET_ROOT    ${SDK_ROOT_PATH}/boards)

# set(BOARD lc_shizhanpai) 

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})   

project(lvgl_display)

target_sources(app PRIVATE 
        src/main.c)                  

