# <CMakeLists.txt> -*- coding: utf-8 -*-
#
# Project LegacyPrintDlg
# Copyright (C) 2025 neige68
#

cmake_minimum_required(VERSION 3.28)
set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_policy(SET CMP0167 OLD)
# スタティックリンク
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(NAME "LegacyPrintDlg")
message(STATUS "NAME: ${NAME}")

project(${NAME} CXX)

#------------------------------------------------------------
#
# boost
#

find_package(Boost 1.88 REQUIRED)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

#------------------------------------------------------------
#
# OWL
#

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

#------------------------------------------------------------

add_subdirectory(src)

# end of <CMakeLists.txt>
