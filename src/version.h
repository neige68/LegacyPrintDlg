// <version.h>
//
// Project LegacyPrintDlg
// Copyright (C) 2025 neige68
// https://github.com/neige68/LegacyPrintDlg
// This program is released under license GPLv3
//
/// \file
/// \brief バージョン定義
//
// Compiler: VC14.3
//

#ifndef version_h
#define version_h

#include <boost/preprocessor/stringize.hpp> // BOOST_PP_STRINGIZE

//------------------------------------------------------------

//
/// リリース番号
//
#define VER_RELEASE                     0
/// リリース番号文字列
#define VER_RELEASE_STR                 "000"

//
/// ビルド番号
//
#define VER_BUILD                       0
//
/// ビルド番号文字列
//
/// "" もしくは ".9"
#define VER_BUILD_STR                   ""

//
/// 製作会社名
//
#define VER_COMPANYNAME                 "neige68"

//
// 製品バージョン
//
#undef VER_PRODUCTVERSION_STR
#undef VER_PRODUCTVERSION
/// 製品バージョン
#define VER_PRODUCTVERSION              0, 0, VER_RELEASE, VER_BUILD
/// 製品バージョン
#define VER_PRODUCTVERSION_STR          "0.00"

//
/// 製品名
//
#define VER_PRODUCTNAME                 "LegacyPrintDlg"

//
/// ファイルバージョン
//
#define VER_FILEVERSION                 0, 0, VER_RELEASE, VER_BUILD
/// ファイルバージョン文字列
#define VER_FILEVERSION_STR             "0.00." VER_RELEASE_STR VER_BUILD_STR

//
/// ファイル説明
//
#define VER_FILEDESCRIPTION             "印刷ダイアログスタイル切り替え"

//
/// \def VER_LEGALCOPYRIGHT_YEAR
/// \brief 著作権発効年
/// \details 例: 2023 or 2023-2024
//
#define VER_LEGALCOPYRIGHT_STARTYEAR    2025
#if !defined(PROJECT_CURRENT_YEAR) || (PROJECT_CURRENT_YEAR == VER_LEGALCOPYRIGHT_STARTYEAR)
# define VER_LEGALCOPYRIGHT_YEAR        BOOST_PP_STRINGIZE(VER_LEGALCOPYRIGHT_STARTYEAR)
#else
# define VER_LEGALCOPYRIGHT_YEAR        BOOST_PP_STRINGIZE(VER_LEGALCOPYRIGHT_STARTYEAR) "-" \
                                        BOOST_PP_STRINGIZE(PROJECT_CURRENT_YEAR)
#endif

//
/// 著作権表示
//
#define VER_LEGALCOPYRIGHT              "Copyright (C) " VER_LEGALCOPYRIGHT_YEAR " " VER_COMPANYNAME

//------------------------------------------------------------

#endif // version_h

// end of <version.h>
