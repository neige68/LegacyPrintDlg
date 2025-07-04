// <main.cpp>
//
// Project LegacyPrintDlg
// Copyright (C) 2025 neige68
//
// Note: main
//
// Compiler: VC14.3
// Library:  OWL7
//

#include "pch.h"
#pragma hdrstop

#include <owl/registry.h>       // owl::TRegKey
#include <owl/string.h>         // owl::TString

#include <filesystem>           // std::filesystem::path
#include <format>               // std::format
#include <stdexcept>            // std::runtime_error
#include <string>               // std::string
#include <vector>               // std::vector

#include "res.rh"               // Resource IDs

//------------------------------------------------------------
//
// ODS - OutputDebugString
//

#ifdef NDEBUG
# define ODS(s) ((void)0)
#else
# define ODS(s)                                         \
    do {                                                \
        TDebugMonitorStarter::Instance();               \
        owl::tostringstream ODS_oss;                    \
        ODS_oss << s << std::endl;                      \
        ::OutputDebugString(ODS_oss.str().c_str());     \
    } while (0)
#endif

class TDebugMonitorStarter {    // デバッグモニタを始動
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
// local
//

// システムのエラーメッセージを取得
// inserts は無視(そのまま)
static std::string GetErrorMessage(DWORD id, DWORD dwLanguageId = 0)
{
    char* buf = 0;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                  | FORMAT_MESSAGE_MAX_WIDTH_MASK,
                  0, id, dwLanguageId, reinterpret_cast<LPTSTR>(&buf), 1, 0);
    std::string result{buf ? buf : ""};
    LocalFree(buf);
    return result;
}

static std::string LoadStr(UINT uID, HINSTANCE hInstance = 0)
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
        if (r < buf.size() - 2) break;
        buf.resize(buf.size() * 2);
    }
    return &buf.at(0);
}

//------------------------------------------------------------

/// PreferLegacyPrintDialog の値の読み込み
bool ReadTheRegKey()
{
    try {
        auto rootKey = HKEY_CURRENT_USER;
        const char* key = "Software\\Microsoft\\Print\\UnifiedPrintDialog";
        const char* value_name = "PreferLegacyPrintDialog";

        owl::TRegKey regKey(rootKey, key, KEY_READ, owl::TRegKey::NoCreate);
        bool result = DWORD(owl::TRegValue(regKey, value_name)) != 0;
        return result;
    }
    catch (const std::exception& x) {
        TRACE("ReadTheRegKey: Exception: " << x.what());
        return false;
    }
    catch (...) {
        throw;
    }
}

/// PreferLegacyPrintDialog の値の書き込み
void WriteTheRegKey(bool legacy, HWND hwnd)
{
    if (ReadTheRegKey() == legacy)
        return;                 // 不要
    std::filesystem::path fname = "UnifiedPrintDialog_Modern.reg";
    if (legacy)
        fname = "UnifiedPrintDialog_Legacy.reg";
    TRACE(_T("WriteTheRegKey: fname: " << fname));
    auto hInst = ShellExecute(hwnd, _T("open"), fname.string().c_str(), nullptr, nullptr, SW_NORMAL);
    auto result = reinterpret_cast<INT_PTR>(hInst);
    if (result <= 32)
        throw std::runtime_error(GetErrorMessage(result));
}

//------------------------------------------------------------
//
// class TMyClientDialog - アプリケーションフレームのクライアントダイアログ
//

class TMyClientDialog : public owl::TDialog {

    // *** constructor ***
public:
    TMyClientDialog() : owl::TDialog(nullptr, IDD_MAIN) {}

    // *** OWL override ***
    void SetupWindow() override;
    void CloseWindow(int retVal = 0) override;
};

void TMyClientDialog::SetupWindow()
{
    owl::TDialog::SetupWindow();
    //
    bool preferLegacyPrintDialog = ReadTheRegKey();
    CheckDlgButton(IDC_LEGACY, preferLegacyPrintDialog);
    CheckDlgButton(IDC_MODERN, !preferLegacyPrintDialog);
};

void TMyClientDialog::CloseWindow(int retVal)
{
    ODS("TMyClientDialog::CloseWindow|retVal=" << retVal);
    if (retVal == IDOK) {
        try {
            if (!CanClose()) return;
            ODS("TMyClientDialog::CloseWindow|Save");
            WriteTheRegKey(static_cast<bool>(IsDlgButtonChecked(IDC_LEGACY)), GetHandle());
        }
        catch (const std::exception& x) {
            MessageBox(x.what(), "error", MB_ICONSTOP | MB_OK);
            return;
        }
        catch (...) {
            Destroy(retVal);
            throw;
        }
    }
    Destroy(retVal);
}

//------------------------------------------------------------
//
// class TMyFrameWindow - アプリケーションのフレームウインドウ
//

class TMyFrameWindow : public owl::TFrameWindow {
public:
    TMyFrameWindow(LPCTSTR title, TWindow* clientWnd)
        : owl::TFrameWindow(/*parent*/0, title, clientWnd, /*shrinkToClient*/true) {
        // 最大化ボタンを無効とする
        Attr.Style |= WS_DLGFRAME | WS_CAPTION | WS_CLIPCHILDREN | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
        Attr.Style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CHILD);
    }
};

//------------------------------------------------------------
//
// class TMyApp - アプリケーションクラス
//

class TMyApp : public owl::TApplication {
public:
    explicit TMyApp(LPCTSTR title) : owl::TApplication(title) {}
    void InitMainWindow() override {
        TMyFrameWindow* frame = new TMyFrameWindow(GetName(), new TMyClientDialog);
        frame->SetIcon(this, IDI_APP);
        frame->SetIconSm(this, IDI_APP);
        SetMainWindow(frame);
    }
};

//------------------------------------------------------------
//
// OwlMain
//

int OwlMain(int argc, TCHAR** argv)
{
    std::string title = "(title not loaded)";
    int result = -1;
    try {
        title = LoadStr(IDS_APPNAME);
        TMyApp app(title.c_str());
        result = app.Run();
        ODS("OwlMain|result=" << result);
    }
    catch (const std::exception& x) {
        MessageBox(0, owl::TString(x.what()), title.c_str(), MB_ICONSTOP | MB_OK);
    }
    catch (...) {
        MessageBox(0, "Unexpected exception.", title.c_str(), MB_ICONSTOP | MB_OK);
    }
    return result;
}

//------------------------------------------------------------

// end of <main.cpp>
