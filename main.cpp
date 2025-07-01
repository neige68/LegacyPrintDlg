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

#include <owl/string.h>         // owl::TString

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
        PostMessage(HWND_BROADCAST, RegisterWindowMessage(_T("StartDebugMonitor")), GetCurrentProcessId(), 0);
        OutputDebugString(_T("StartDebugMonitor\r\n"));
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
static owl::tstring GetErrorMessage(DWORD id, DWORD dwLanguageId = 0)
{
    TCHAR* buf = 0;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                  | FORMAT_MESSAGE_MAX_WIDTH_MASK,
                  0, id, dwLanguageId, (LPTSTR)&buf, 1, 0);
    owl::tstring result(buf ? buf : _T(""));
    LocalFree(buf);
    return result;
}

static owl::tstring LoadStr(UINT uID, HINSTANCE hInstance = 0)
{
    std::vector<TCHAR> buf(64);
    for (;;) {
        int r = LoadString(hInstance, uID, &buf.at(0), buf.size());
        if (r == 0) {
            if (DWORD e = GetLastError())
                throw std::runtime_error{owl::TString{_T("LoadStr: ") + GetErrorMessage(e)}};
            throw std::runtime_error("LoadStr: no string resource. (id=" + std::to_string(uID) + ", hInstance="
                                     + std::to_string(reinterpret_cast<INT_PTR>(hInstance)) + ")");
        }
        if (r < buf.size() - 2) break;
        buf.resize(buf.size() * 2);
    }
    return &buf.at(0);
}

//------------------------------------------------------------
//
// struct TMyClientDialogXfer - TMyClientDialog の転送バッファ
//

#pragma pack(push, 1)
struct TMyClientDialogXfer {
    // mark
    //
    TMyClientDialogXfer() { Load(); }
    void Save();
    void Load();
};
#pragma pack(pop)

void TMyClientDialogXfer::Save()
{
    // mark
}

void TMyClientDialogXfer::Load()
{
    try {
        // mark
    }
    catch (const std::exception& x) {
        MessageBox(0, owl::TString(x.what()), _T("TMyClientDialogXfer::Load"), MB_ICONSTOP | MB_OK);
    }
}

//------------------------------------------------------------
//
// class TMyClientDialog - アプリケーションフレームのクライアントダイアログ
//

class TMyClientDialog : public owl::TDialog {

    // *** static data ***
public:
    static TMyClientDialogXfer Data;

    // *** constructor ***
    TMyClientDialog();

    // *** response ***
    // mark

    // *** OWL override ***
    void SetupWindow() override;
    bool CanClose() override;
    void CloseWindow(int retVal = 0) override;
    bool IdleAction(long idleCount) override;

    // *** data ***
private:
    // mark
    bool FirstIdle;

    DECLARE_RESPONSE_TABLE(TMyClientDialog);
};

//static
TMyClientDialogXfer TMyClientDialog::Data;

DEFINE_RESPONSE_TABLE1(TMyClientDialog, owl::TDialog)
END_RESPONSE_TABLE;

TMyClientDialog::TMyClientDialog()
    : owl::TDialog(/*parent*/0, /*resId*/IDD_MAIN)
{
    // mark
    //SetTransferBuffer(&Data);
}

void TMyClientDialog::SetupWindow()
{
    owl::TDialog::SetupWindow();
    //
    FirstIdle = true;
};

bool TMyClientDialog::CanClose()
{
    ODS("TMyClientDialog::CanClose");
    // mark
    return owl::TDialog::CanClose();
}

void TMyClientDialog::CloseWindow(int retVal)
{
    ODS("TMyClientDialog::CloseWindow|retVal=" << retVal);
    if (retVal == IDOK) {
        try {
            if (!CanClose()) return;
            ODS("TMyClientDialog::CloseWindow|Save");
            //TransferData(owl::tdGetData);
            Data.Save();
        }
        catch (const std::exception& x) {
            MessageBox(owl::TString(x.what()), _T("error"), MB_ICONSTOP | MB_OK);
            return;
        }
        catch (...) {
            Destroy(retVal);
            throw;
        }
    }
    Destroy(retVal);
}

bool TMyClientDialog::IdleAction(long idleCount)
{
    if (FirstIdle) {
        FirstIdle = false;
        // mark
        ODS("TMyClientDialog::IdleAction-first");
        //
        // Window に設定されているフォントを ODS 出力してみる
        //
        HFONT hFont = GetWindowFont(); // システムフォントを使用している場合は 0
        ODS("hFont=" << hFont);
        if (hFont) {
            owl::TFont font(hFont);
            ODS(_T("font.FaceName=") << font.GetFaceName());
            ODS(_T("font.Height=") << font.GetHeight());
            ODS(_T("font.AveWidth=") << font.GetAveWidth());
        }
        else {
            // システムフォントを出力してみる
            NONCLIENTMETRICS m = { sizeof(NONCLIENTMETRICS), };
            if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof m, &m, 0))
                ODS("SystemParametersInfoA failed.");
            else {
#define ODS_FONT(x) ODS(#x "=#<" << x.lfFaceName << "," << x.lfHeight << ">")
                ODS_FONT(m.lfCaptionFont);
                ODS_FONT(m.lfSmCaptionFont);
                ODS_FONT(m.lfMenuFont);
                ODS_FONT(m.lfMessageFont);
            }
        }
    }
    return TDialog::IdleAction(idleCount);
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
    void InitMainWindow() override;

    // アプリケーションはいつでも終われるようにする
    //
    // 本来はセーブしていないデータをセーブし、キャンセルも可能とすつ
    // ためのものである。
    //
    // このアプリケーションでは、ダイアログクラスの CanClose はダイア
    // ログのデータの有効性をチェックするので別物となる。
    bool CanClose() override { return true; }
};


void TMyApp::InitMainWindow()
{
    TMyFrameWindow* frame = new TMyFrameWindow(GetName(), new TMyClientDialog);
    frame->SetIcon(this, IDI_APP);
    frame->SetIconSm(this, IDI_APP);
    SetMainWindow(frame);
}

//------------------------------------------------------------
//
// OwlMain
//

int OwlMain(int argc, TCHAR** argv)
{
    owl::tstring title = _T("(title not loaded)");
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
        MessageBox(0, _T("Unexpected exception."), title.c_str(), MB_ICONSTOP | MB_OK);
    }
    return result;
}

//------------------------------------------------------------

// end of <main.cpp>
