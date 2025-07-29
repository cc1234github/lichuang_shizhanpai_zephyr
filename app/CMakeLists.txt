#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# 指定板子，build 时直接使用west build
set(BOARD lc_shizhanpai)            
# 设定 BOARD_ROOT 指向工作空间的目录 
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
list(APPEND BOARD_ROOT ${PARENT_DIR})


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})   # 查找 Zephyr 包
project(my_led LANGUAGES C)                             # 定义工程
target_sources(app PRIVATE src/main.c)                  # 源文件
