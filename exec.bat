@rem exec.bat -*- coding: cp932-dos -*-
@echo off
rem
rem Project LegacyPrintDlg
rem Copyright (C) 2025 neige68
rem https://github.com/neige68/LegacyPrintDlg
rem This program is released under license GPLv3
rem
rem Windows: 10 and later
rem

setlocal
pushd %~dp0
set @config=debug
set @name=LegacyPrintDlg
set @verbose=
:loop
if "%1"=="--" goto optbreak
if "%1"=="r" goto rel
if "%1"=="-r" goto rel
if "%1"=="rel" goto rel
if "%1"=="d" goto deb
if "%1"=="-d" goto deb
if "%1"=="deb" goto deb
if "%1"=="v" goto verbose
if "%1"=="-v" goto verbose
if "%1"=="h" goto help
if "%1"=="-h" goto help
if "%1"=="--help" goto help
goto optend
:rel
set @config=release
shift
goto loop
:deb
set @config=debug
shift
goto loop
:verbose
set @verbose=t
shift
goto loop
:optbreak
shift
:optend
if not "%1"=="" set @name=%1
if "%@name%"=="" goto help
set @exe=%@name%.exe
if not "%@verbose%"=="" echo INFO: %@exe%
shift
shift
pushd build\src\%@config%
%@exe% %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
popd
goto end
:help
echo usage: exec [v] [deb/rel] [--] name
:end
popd
rem end of exec.bat
