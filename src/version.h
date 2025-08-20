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

#define VER_COMPANYNAME                 "neige68"

#undef VER_PRODUCTVERSION_STR
#undef VER_PRODUCTVERSION
#define VER_PRODUCTVERSION              1, 0, 0, 0
#define VER_PRODUCTVERSION_STR          "1.0.0"

#define VER_PRODUCTNAME                 "LegacyPrintDlg"

#define VER_FILEVERSION                 VER_PRODUCTVERSION
#define VER_FILEVERSION_STR             VER_PRODUCTVERSION_STR

#define VER_FILEDESCRIPTION             "印刷ダイアログの新旧切替"

#define VER_LEGALCOPYRIGHT_STARTYEAR    2025
#if !defined(PROJECT_CURRENT_YEAR) || (PROJECT_CURRENT_YEAR == VER_LEGALCOPYRIGHT_STARTYEAR)
# define VER_LEGALCOPYRIGHT_YEAR        BOOST_PP_STRINGIZE(VER_LEGALCOPYRIGHT_STARTYEAR)
#else
# define VER_LEGALCOPYRIGHT_YEAR        BOOST_PP_STRINGIZE(VER_LEGALCOPYRIGHT_STARTYEAR) "-" \
                                        BOOST_PP_STRINGIZE(PROJECT_CURRENT_YEAR)
#endif

#define VER_LEGALCOPYRIGHT              "Copyright (C) " VER_LEGALCOPYRIGHT_YEAR " " VER_COMPANYNAME

//------------------------------------------------------------

#endif // version_h

// end of <version.h>
