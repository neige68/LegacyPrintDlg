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

class TDebugMonitorStarter {    // �f�o�b�O���j�^���n��
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

// �V�X�e���̃G���[���b�Z�[�W���擾
// inserts �͖���(���̂܂�)
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

/// PreferLegacyPrintDialog �̒l�̓ǂݍ���
bool ReadTheRegKey()
{
    try {
        auto rootKey = HKEY_CURRENT_USER;
        const TCHAR* key = _T("Software\\Microsoft\\Print\\UnifiedPrintDialog");
        const TCHAR* value_name = _T("PreferLegacyPrintDialog");

        owl::TRegKey regKey(rootKey, key, KEY_READ, owl::TRegKey::NoCreate);
        bool result = DWORD(owl::TRegValue(regKey, value_name)) != 0;
        return result;
    }
    catch (const std::exception& x) {
        TRACE(_T("ReadTheRegKey: Exception: ") << x.what());
        return false;
    }
    catch (...) {
        throw;
    }
}

/// PreferLegacyPrintDialog �̒l�̏�������
void WriteTheRegKey(bool legacy, HWND hwnd)
{
    if (ReadTheRegKey() == legacy)
        return;                 // �s�v
    std::filesystem::path fname = "UnifiedPrintDialog_Modern.reg";
    if (legacy)
        fname = "UnifiedPrintDialog_Legacy.reg";
    TRACE(_T("WriteTheRegKey: fname: " << fname));
    auto hInst = ShellExecute(hwnd, _T("open"), fname.c_str(), nullptr, nullptr, SW_NORMAL);
    auto result = reinterpret_cast<INT_PTR>(hInst);
    if (result <= 32)
        throw std::runtime_error(std::filesystem::path(GetErrorMessage(result)).string());
}

//------------------------------------------------------------
//
// class TMyClientDialog - �A�v���P�[�V�����t���[���̃N���C�A���g�_�C�A���O
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

//------------------------------------------------------------
//
// class TMyFrameWindow - �A�v���P�[�V�����̃t���[���E�C���h�E
//

class TMyFrameWindow : public owl::TFrameWindow {
public:
    TMyFrameWindow(LPCTSTR title, TWindow* clientWnd)
        : owl::TFrameWindow(/*parent*/0, title, clientWnd, /*shrinkToClient*/true) {
        // �ő剻�{�^���𖳌��Ƃ���
        Attr.Style |= WS_DLGFRAME | WS_CAPTION | WS_CLIPCHILDREN | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
        Attr.Style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CHILD);
    }
};

//------------------------------------------------------------
//
// class TMyApp - �A�v���P�[�V�����N���X
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
