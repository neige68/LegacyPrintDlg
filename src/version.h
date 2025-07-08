// <version.h>
//
// Project LegacyPrintDlg
// Copyright (C) 2025 neige68
// https://github.com/neige68/LegacyPrintDlg
// This program is released under license GPLv3
//
/// \file
/// \brief �o�[�W������`
//
// Compiler: VC14.3
//

#ifndef version_h
#define version_h

#include <boost/preprocessor/stringize.hpp> // BOOST_PP_STRINGIZE

//------------------------------------------------------------

//
/// �����[�X�ԍ�
//
#define VER_RELEASE                     0
/// �����[�X�ԍ�������
#define VER_RELEASE_STR                 "000"

//
/// �r���h�ԍ�
//
#define VER_BUILD                       0
//
/// �r���h�ԍ�������
//
/// "" �������� ".9"
#define VER_BUILD_STR                   ""

//
/// �����Ж�
//
#define VER_COMPANYNAME                 "neige68"

//
// ���i�o�[�W����
//
#undef VER_PRODUCTVERSION_STR
#undef VER_PRODUCTVERSION
/// ���i�o�[�W����
#define VER_PRODUCTVERSION              0, 0, VER_RELEASE, VER_BUILD
/// ���i�o�[�W����
#define VER_PRODUCTVERSION_STR          "0.00"

//
/// ���i��
//
#define VER_PRODUCTNAME                 "LegacyPrintDlg"

//
/// �t�@�C���o�[�W����
//
#define VER_FILEVERSION                 0, 0, VER_RELEASE, VER_BUILD
/// �t�@�C���o�[�W����������
#define VER_FILEVERSION_STR             "0.00." VER_RELEASE_STR VER_BUILD_STR

//
/// �t�@�C������
//
#define VER_FILEDESCRIPTION             "����_�C�A���O�X�^�C���؂�ւ�"

//
/// \def VER_LEGALCOPYRIGHT_YEAR
/// \brief ���쌠�����N
/// \details ��: 2023 or 2023-2024
//
#define VER_LEGALCOPYRIGHT_STARTYEAR    2025
#if !defined(PROJECT_CURRENT_YEAR) || (PROJECT_CURRENT_YEAR == VER_LEGALCOPYRIGHT_STARTYEAR)
# define VER_LEGALCOPYRIGHT_YEAR        BOOST_PP_STRINGIZE(VER_LEGALCOPYRIGHT_STARTYEAR)
#else
# define VER_LEGALCOPYRIGHT_YEAR        BOOST_PP_STRINGIZE(VER_LEGALCOPYRIGHT_STARTYEAR) "-" \
                                        BOOST_PP_STRINGIZE(PROJECT_CURRENT_YEAR)
#endif

//
/// ���쌠�\��
//
#define VER_LEGALCOPYRIGHT              "Copyright (C) " VER_LEGALCOPYRIGHT_YEAR " " VER_COMPANYNAME

//------------------------------------------------------------

#endif // version_h

// end of <version.h>
