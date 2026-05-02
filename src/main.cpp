// <main.cpp> -*- coding: utf-8-with-signature -*-
//
// Project LegacyPrintDlg
// Copyright (C) 2025 neige68
// https://github.com/neige68/LegacyPrintDlg
// This program is released under license GPLv3
//
/// \file
/// \brief メインモジュール
//
// Compiler: VC14.3
//

#include "pch.h"
#pragma hdrstop

#include "version.h"            // VER_PRODUCTVERSION_STR

#include <bcrypt.h>             // BCryptHashData

#include <algorithm>            // std::equal
#include <array>                // std::array
#include <filesystem>           // std::filesystem::path
#include <format>               // std::format
#include <fstream>              // std::ifstream
#include <sstream>              // std::ostringstream
#include <stdexcept>            // std::runtime_error
#include <string>               // std::string
#include <vector>               // std::vector

#include "res.rh"               // Resource IDs

//------------------------------------------------------------

static HINSTANCE g_hInst = nullptr;

//------------------------------------------------------------

#if !defined(NDEBUG)
#define TRACE(msg) do { \
    std::ostringstream oss_; oss_ << msg << "\r\n"; \
    OutputDebugStringA(oss_.str().c_str()); \
} while(0)
#else
#define TRACE(msg) ((void)0)
#endif

//------------------------------------------------------------
//
/// デバッグモニター起動
//
/// StartDebugMonitor Window Message を受信することで OutputDebugString で出力
/// されたメッセージを取得するツールを起動するためのシングルトンクラスです。
/// そのようなツールが存在しましたが名前は忘れました。
/// 現在は自作のツールを使用してます。
//

class TDebugMonitorStarter {
public:
    static TDebugMonitorStarter& Instance();
private:
    TDebugMonitorStarter() {
        PostMessage(HWND_BROADCAST, RegisterWindowMessage("StartDebugMonitor"), GetCurrentProcessId(), 0);
        OutputDebugString("StartDebugMonitor\r\n");
    }
};

//static
TDebugMonitorStarter& TDebugMonitorStarter::Instance()
{
    static TDebugMonitorStarter instance;
    return instance;
};

//------------------------------------------------------------
//
/// システムのエラーメッセージを取得
//
/// inserts は無視(そのまま)
//

std::string GetErrorMessage(DWORD id, DWORD dwLanguageId = 0)
{
    char* buf = 0;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                  | FORMAT_MESSAGE_MAX_WIDTH_MASK,
                  0, id, dwLanguageId, reinterpret_cast<LPTSTR>(&buf), 1, 0);
    std::string result{buf ? buf : ""};
    LocalFree(buf);
    return result;
}

//------------------------------------------------------------
//
/// リソースの文字列を取得
//

std::string LoadStr(UINT uID, HINSTANCE hInstance = nullptr)
{
    std::vector<char> buf(64);
    for (;;) {
        int r = LoadString(hInstance, uID, &buf.at(0), buf.size());
        if (r == 0) {
            if (DWORD e = GetLastError())
                throw std::runtime_error{"LoadStr: " + GetErrorMessage(e)};
            throw std::runtime_error{std::format("LoadStr: no string resource. (id={}, hInstance={})",
                                                 uID, reinterpret_cast<INT_PTR>(hInstance))};
        }
        if (r < static_cast<int>(buf.size()) - 2) break;
        buf.resize(buf.size() * 2);
    }
    return &buf.at(0);
}

//------------------------------------------------------------
//
/// レジストリ PreferLegacyPrintDialog の値の読み込み
//

bool ReadTheRegKey()
{
    const char* key = "Software\\Microsoft\\Print\\UnifiedPrintDialog";
    const char* value_name = "PreferLegacyPrintDialog";
    HKEY hKey = nullptr;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return false;
    DWORD value = 0;
    DWORD size = sizeof(value);
    LONG r = RegQueryValueExA(hKey, value_name, nullptr, nullptr, reinterpret_cast<LPBYTE>(&value), &size);
    RegCloseKey(hKey);
    if (r != ERROR_SUCCESS)
        return false;
    return value != 0;
}

//------------------------------------------------------------
//
/// レジストリ PreferLegacyPrintDialog の値の書き込み
//

void WriteTheRegKey(bool legacy, HWND hwnd)
{
    if (ReadTheRegKey() == legacy)
        return;                 // 不要
    //
    // moden/legacy それぞれのレジストリファイルの SHA-512 ハッシュ値
    //
    // レジストリファイルの改竄検知のためハッシュをチェックする。
    //
    // Windows コマンドプロンプトでは
    //   certutil -hashfile <filename> SHA512
    // で計算できる。
    //
    const char* moden_fname = "UnifiedPrintDialog_Modern.reg";
    const char* moden_hash =
        "\x88\xaa\x7d\x68\xd3\x84\xab\x2e\x0f\x46\x2c\x3b\x96\xa3\xfd\x34"
        "\x8b\x2c\x55\x50\xb1\xe2\x07\x8a\xa8\x19\xb8\x3c\xfc\x4d\xe8\x5d"
        "\xcd\x57\x22\xa0\x19\xe5\x17\x3e\x98\x56\xad\x20\x6b\x01\xf8\x56"
        "\xcf\x8f\x0d\x63\x54\x22\x75\xb1\x23\xb0\x74\xba\x92\xc3\xe9\xd4";
    const char* legacy_fname = "UnifiedPrintDialog_Legacy.reg";
    const char* legacy_hash =
        "\x62\xfc\xd2\x0d\x3b\xf3\xa0\x3a\xdf\x6e\x6c\x24\x77\x90\x12\xd6"
        "\xc9\x84\xa4\xc5\x9f\x3b\x39\x33\x6e\xa7\x1d\x22\xc5\x92\xd3\xe5"
        "\xa5\x84\x0c\x2f\xf2\x53\xcf\xe2\x3a\xdf\xc1\xe1\x70\xbe\x87\x24"
        "\x9c\xab\xcd\x7b\x28\x5c\xd0\x94\xb6\x45\x6b\x0f\xf3\x33\xa1\x25";
    // 引数 legacy の真偽によりどちらかを選択
    std::filesystem::path fname = moden_fname;
    const char* hash = moden_hash;
    if (legacy) {
        fname = legacy_fname;
        hash = legacy_hash;
    }
    TRACE("WriteTheRegKey: fname: " << fname);
    // ファイルの存在確認
    if (!std::filesystem::exists(fname))
        throw std::runtime_error("File not found: " + fname.string());
    // ハッシュ確認
    {
        BCRYPT_ALG_HANDLE hAlg = nullptr;
        BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA512_ALGORITHM, nullptr, 0);
        BCRYPT_HASH_HANDLE hHash = nullptr;
        BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0);
        std::ifstream ifs{fname, std::ios::binary};
        if (!ifs) {
            BCryptDestroyHash(hHash);
            BCryptCloseAlgorithmProvider(hAlg, 0);
            throw std::runtime_error("Cannot open file: " + fname.string());
        }
        std::vector<char> buffer(512);
        while (ifs.read(buffer.data(), buffer.size()) || ifs.gcount())
            BCryptHashData(hHash, reinterpret_cast<PUCHAR>(buffer.data()), static_cast<ULONG>(ifs.gcount()), 0);
        std::array<unsigned char, 64> digest{};
        BCryptFinishHash(hHash, digest.data(), static_cast<ULONG>(digest.size()), 0);
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        // バイナリとして比較
        if (!std::equal(digest.begin(), digest.end(), reinterpret_cast<const unsigned char*>(hash)))
            throw std::runtime_error("File corrupted: " + fname.string());
    }
    // 実行
    auto hInst = ShellExecute(hwnd, "open", fname.string().c_str(), nullptr, nullptr, SW_NORMAL);
    if (auto ec = reinterpret_cast<INT_PTR>(hInst); ec <= 32) // エラー
        throw std::runtime_error(GetErrorMessage(static_cast<DWORD>(ec)));
}

//------------------------------------------------------------
//
/// アプリについてのダイアログプロシージャ
//

INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_INITDIALOG:
        {
            // バージョン文字列をアプリ名に追加
            wchar_t buf[256] = {};
            GetDlgItemTextW(hwnd, IDC_APPNAME, buf, 256);
            const char* ver = " Ver. " VER_PRODUCTVERSION_STR;
            std::wstring wver(ver, ver + strlen(ver));
            SetDlgItemTextW(hwnd, IDC_APPNAME, (std::wstring(buf) + wver).c_str());
            // 説明テキスト設定
            const wchar_t* description =
                L"Windows 11 (22H2 以降) では、Win32 アプリに対して新しい印刷ダイアログが"
                L"使われるようになったことで、一部設定が正しく反映されないなどの問題が"
                L"起きることがあります。\r\n\r\n"
                L"本アプリ LegacyPrintDlg は、印刷ダイアログを旧型に切り替えることで、"
                L"アプリが指定した印刷設定が確実に反映されるようにするためのツールです。\r\n\r\n"
                L"レジストリ操作で行いますが、それを GUI 操作で簡単に行えるようにしました。";
            SetDlgItemTextW(hwnd, IDC_DESCRIPTION, description);
        }
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwnd, LOWORD(wParam));
            return TRUE;
        }
        break;
    case WM_NOTIFY:
        {
            auto* pnm = reinterpret_cast<NMHDR*>(lParam);
            if (pnm->idFrom == IDC_GITHUB && pnm->code == NM_CLICK) {
                ShellExecuteA(nullptr, "open", "https://github.com/neige68/LegacyPrintDlg/",
                              nullptr, nullptr, SW_SHOW);
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
}

//------------------------------------------------------------
//
/// メインダイアログプロシージャ
//

INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_INITDIALOG:
        {
            HICON hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_APP));
            SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
            SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
            // レジストリから値を読み込み、それによりラジオボタンのチェック状態を設定
            bool preferLegacyPrintDialog = ReadTheRegKey();
            CheckDlgButton(hwnd, IDC_LEGACY, preferLegacyPrintDialog ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hwnd, IDC_MODERN, preferLegacyPrintDialog ? BST_UNCHECKED : BST_CHECKED);
        }
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            try {
                // ラジオボタンのチェック状態により、レジストリに書き込む
                WriteTheRegKey(IsDlgButtonChecked(hwnd, IDC_LEGACY) == BST_CHECKED, hwnd);
            }
            catch (const std::exception& x) {
                MessageBoxA(hwnd, x.what(), "error", MB_ICONSTOP | MB_OK);
                return TRUE;
            }
            EndDialog(hwnd, IDOK);
            return TRUE;
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            return TRUE;
        }
        break;
    case WM_NOTIFY:
        {
            auto* pnm = reinterpret_cast<NMHDR*>(lParam);
            if (pnm->idFrom == IDC_ABOUT && pnm->code == NM_CLICK) {
                DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc, 0);
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
}

//------------------------------------------------------------
//
/// エントリポイント
//

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    g_hInst = hInstance;
    std::string title = "(title not loaded)";
    try {
#if !defined(NDEBUG)
        TDebugMonitorStarter::Instance();
#endif
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_LINK_CLASS };
        InitCommonControlsEx(&icc);
        title = LoadStr(IDS_APPNAME, hInstance);
        DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN),
                       nullptr, MainDlgProc, 0);
    }
    catch (const std::exception& x) {
        MessageBoxA(nullptr, x.what(), title.c_str(), MB_ICONSTOP | MB_OK);
    }
    catch (...) {
        MessageBoxA(nullptr, "Unexpected exception.", title.c_str(), MB_ICONSTOP | MB_OK);
    }
    return 0;
}

//------------------------------------------------------------

// end of <main.cpp>
