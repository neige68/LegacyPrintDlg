# <CMakeLists.txt> -*- coding: utf-8 -*-
#
# Project LegacyPrintDlg
# Copyright (C) 2025 neige68
#

cmake_minimum_required(VERSION 3.31)
cmake_policy(SET CMP0167 OLD)
set(CMAKE_VERBOSE_MAKEFILE ON)
# スタティックリンク
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(NAME "LegacyPrintDlg")
message(STATUS "NAME: ${NAME}")
project(${NAME} CXX)

#------------------------------------------------------------
#
# boost
#

# BOOST を探す
# C: ドライブ以外の場合は 環境変数 CMAKE_PREFIX_PATH 等をセットしてください
find_package(Boost 1.88 REQUIRED COMPONENTS date_time system)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

#------------------------------------------------------------
#
# OWL
#

# Include/Link Dir # add_executable より前じゃないとだめらしい
set(OWL $ENV{OWLROOT})
message(STATUS "OWL: ${OWL}")
include_directories(${OWL}/include)
link_directories(${OWL}/lib)

#------------------------------------------------------------

# コンパイルオプション
set(CMAKE_CXX_FLAGS "/EHsc /MP /Yu\"pch.h\"")
set(CMAKE_CXX_STANDARD "20")
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D__TRACE /D__WARN /D__DEBUG=2")

string(TIMESTAMP CURRENT_YEAR "%Y")
add_compile_definitions(PROJECT_CURRENT_YEAR=${CURRENT_YEAR})

add_link_options(/ENTRY:WinMainCRTStartup)

add_executable(${NAME} WIN32 main.cpp res.rc resource.rc pch.cpp)

# プリコンパイルヘッダを生成
set_source_files_properties(pch.cpp PROPERTIES COMPILE_FLAGS "/Yc\"pch.h\"")

# end of <CMakeLists.txt>
