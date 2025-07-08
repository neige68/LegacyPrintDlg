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

rem @VC: VC �� vcvarsall.bat �̂���f�B���N�g�����w��
set @VC=%VC143%
set @CMAKEOPT=-G "Visual Studio 17 2022" -A x64

set @exec_cmake=
set @bdir=..\build
set @build_deb=t
set @build_rel=t

:opt_loop
if "%1"=="" goto opt_end
rem ���� cm: cmake �����s
if "%1"=="cm" set @exec_cmake=t
if "%1"=="cm" goto opt_next
rem ���� rebuild: build �f�B���N�g������蒼��
if "%1"=="rebuild" goto opt_rebuild
if "%1"=="reb" goto opt_rebuild
if "%1"=="re" goto opt_rebuild
if "%1"=="clean" goto opt_rebuild
rem ���� deb: �f�o�b�O�ł̂݃r���h
if "%1"=="d" set @build_rel=
if "%1"=="d" goto opt_next
if "%1"=="deb" set @build_rel=
if "%1"=="deb" goto opt_next
rem ���� rel: �����[�X�ł̂݃r���h
if "%1"=="r" set @build_deb=
if "%1"=="r" goto opt_next
if "%1"=="rel" set @build_deb=
if "%1"=="rel" goto opt_next
echo WARN: build.bat: %1 �𖳎����܂�.
:opt_next
shift
goto opt_loop
:opt_rebuild
if not exist %@bdir% echo INFO: build.bat: %@bdir% �͂���܂���.
if exist %@bdir% echo INFO: build.bat: %@bdir% ���폜���Ă��܂�...
if exist %@bdir% rmdir /s /q %@bdir%
if exist %@bdir% echo ERROR: build.bat: %@bdir% �̍폜���ł��܂���ł���.
if exist %@bdir% goto end
set @exec_cmake=t
shift
goto opt_loop
:opt_end

rem *** vcvars ***
if not "%VCToolsVersion%"=="" echo INFO: build.bat: VCToolsVersion ���ݒ肳��Ă��܂�. ���̂܂܎��s���܂�.
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
echo ERROR: build.bat: �G���[������܂�.
popd
popd
exit /b 1

rem *** end ***
:end
popd

rem end of build.bat
