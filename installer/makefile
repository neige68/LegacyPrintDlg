# <makefile> -*-makefile-*-
#
# Project LegacyPrintDlg
# Copyright (C) 2025 neige68
# https://github.com/neige68/LegacyPrintDlg
# This program is released under license GPLv3
#
# make:      GNU make
#

NAME = LegacyPrintDlg
BUILD_DIR = ..\build\installer

.SUFFIXES: .wxs .wixobj

.PHONY : all
all : $(BUILD_DIR)\$(NAME).msi

#
# 以下の記法は GNU make 専用
# Borland make では(少なくとも5.2では)不可
#
# 自動変数:
#     $< : 最初の依存ファイル名
#     $? : ターゲットより新しいすべての依存ファイル名
#     $^ : すべての依存ファイル名
#     $+ : すべての依存ファイル名(順序維持)
#     $@ : ターゲットファイル名
#     $% : ターゲットがアーカイブメンバだったときのターゲットメンバ名
#     $* : 暗黙のルールで一致したターゲットの語幹

$(BUILD_DIR)\Setup.wixobj : Setup.wxs
	cmd /c "if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)"
	candle -o $@ $<

$(BUILD_DIR)\$(NAME).msi : WixUI_Alt_ja-jp.wxl $(BUILD_DIR)\Setup.wixobj makefile License.rtf ..\build\src\Release\LegacyPrintDlg.exe
	light -ext WixUIExtension -cultures:ja-jp -loc WixUI_Alt_ja-jp.wxl -o $@ $(BUILD_DIR)\Setup.wixobj 

# Local variables:
# indent-tabs-mode: t
# End: 
