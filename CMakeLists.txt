# <CMakeLists.txt> -*- coding: utf-8 -*-
#
# Project LegacyPrintDlg
# Copyright (C) 2025 neige68
# https://github.com/neige68/LegacyPrintDlg
# This program is released under license GPLv3
#

cmake_minimum_required(VERSION 3.28)
set(CMAKE_VERBOSE_MAKEFILE ON)
# スタティックリンク
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(NAME "LegacyPrintDlg")
message(STATUS "NAME: ${NAME}")

project(${NAME} CXX)

#------------------------------------------------------------

# コンパイルオプション
set(CMAKE_CXX_FLAGS "/EHsc /MP /Yu\"pch.h\"")
set(CMAKE_CXX_STANDARD "20")
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

string(TIMESTAMP CURRENT_YEAR "%Y")
add_compile_definitions(PROJECT_CURRENT_YEAR=${CURRENT_YEAR})

#------------------------------------------------------------

add_subdirectory(src)

# end of <CMakeLists.txt>
