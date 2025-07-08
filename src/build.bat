@rem build.bat -*- coding: cp932-dos -*-
@echo off
rem
rem Project LegacyPrintDlg
rem Copyright (C) 2025 neige68
rem
rem Note: build
rem
rem Windows: 10 and later
rem

setlocal
pushd %~dp0

rem @VC: VC の vcvarsall.bat のあるディレクトリを指定
set @VC=%VC143%
set @CMAKEOPT=-G "Visual Studio 17 2022" -A x64

set @exec_cmake=
set @bdir=..\build
set @build_deb=t
set @build_rel=t

:opt_loop
if "%1"=="" goto opt_end
rem 引数 cm: cmake を実行
if "%1"=="cm" set @exec_cmake=t
if "%1"=="cm" goto opt_next
rem 引数 rebuild: build ディレクトリを作り直す
if "%1"=="rebuild" goto opt_rebuild
if "%1"=="reb" goto opt_rebuild
if "%1"=="re" goto opt_rebuild
if "%1"=="clean" goto opt_rebuild
rem 引数 deb: デバッグ版のみビルド
if "%1"=="d" set @build_rel=
if "%1"=="d" goto opt_next
if "%1"=="deb" set @build_rel=
if "%1"=="deb" goto opt_next
rem 引数 rel: リリース版のみビルド
if "%1"=="r" set @build_deb=
if "%1"=="r" goto opt_next
if "%1"=="rel" set @build_deb=
if "%1"=="rel" goto opt_next
echo WARN: build.bat: %1 を無視します.
:opt_next
shift
goto opt_loop
:opt_rebuild
if not exist %@bdir% echo INFO: build.bat: %@bdir% はありません.
if exist %@bdir% echo INFO: build.bat: %@bdir% を削除しています...
if exist %@bdir% rmdir /s /q %@bdir%
if exist %@bdir% echo ERROR: build.bat: %@bdir% の削除ができませんでした.
if exist %@bdir% goto end
set @exec_cmake=t
shift
goto opt_loop
:opt_end

rem *** vcvars ***
if not "%VCToolsVersion%"=="" echo INFO: build.bat: VCToolsVersion が設定されています. このまま実行します.
if "%VCToolsVersion%"=="" call "%@VC%\vcvarsall.bat" x64

echo INFO: The following lines are encoded in utf-8-dos.

rem *** build directory ***
if not exist %@bdir% mkdir %@bdir%
pushd %@bdir%

rem *** cmake ***
if not exist ALL_BUILD.vcxproj set @exec_cmake=t
if not "%@exec_cmake%"=="" cmake %@CMAKEOPT% ../src

rem *** debug ***
if "%@build_deb%"=="" goto skip_debug
msbuild ALL_BUILD.vcxproj /p:Configuration=Debug /m
if errorlevel 1 goto err
echo INFO: build.bat: msbuild Debug Done.

rem *** release ***
if "%@build_rel%"=="" goto skip_release
msbuild ALL_BUILD.vcxproj /p:Configuration=Release /m
if errorlevel 1 goto err
echo INFO: build.bat: msbuild Release Done.

rem *** end build directory ***
echo INFO: The following lines are encoded in cp932-dos.
popd
goto end

rem *** error ***
:err
echo INFO: The following lines are encoded in cp932-dos.
sleep 1
echo ERROR: build.bat: エラーがあります.
popd
popd
exit /b 1

rem *** end ***
:end
popd

rem end of build.bat
