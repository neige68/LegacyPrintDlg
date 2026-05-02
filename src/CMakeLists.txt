# <CMakeLists.txt> -*- coding: utf-8 -*-
#
# Project LegacyPrintDlg
# Copyright (C) 2025 neige68
# https://github.com/neige68/LegacyPrintDlg
# This program is released under license GPLv3
#

add_executable(${NAME} WIN32 main.cpp res.rc resource.rc pch.cpp)

target_link_libraries(${NAME} PRIVATE comctl32)
target_link_options(${NAME} PRIVATE /MANIFEST:NO)

# マニフェスト生成をしない
set_target_properties(${NAME} PROPERTIES
  VS_GLOBAL_ManifestToolEnableExecutionLevel "false"
  )

# プリコンパイルヘッダを生成
set_source_files_properties(pch.cpp PROPERTIES COMPILE_FLAGS "/Yc\"pch.h\"")

# 出力ディレクトリにコピー
add_custom_command(
    TARGET ${NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/assets/UnifiedPrintDialog_Legacy.reg
        $<TARGET_FILE_DIR:${NAME}>/UnifiedPrintDialog_Legacy.reg
)
add_custom_command(
    TARGET ${NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/assets/UnifiedPrintDialog_Modern.reg
        $<TARGET_FILE_DIR:${NAME}>/UnifiedPrintDialog_Modern.reg
)

# end of <CMakeLists.txt>
