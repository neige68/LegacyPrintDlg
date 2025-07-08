// <main.cpp>
//
// Project LegacyPrintDlg
// Copyright (C) 2025 neige68
//
/// \file
/// \brief ���C�����W���[��
//
// Compiler: VC14.3
// Library:  OWL7
//

#include "pch.h"
#pragma hdrstop

#include <boost/hash2/sha2.hpp> // boost::hash2::sha2_512

#include <owl/registry.h>       // owl::TRegKey
#include <owl/string.h>         // owl::TString

#include <algorithm>            // std::equal
#include <filesystem>           // std::filesystem::path
#include <format>               // std::format
#include <stdexcept>            // std::runtime_error
#include <string>               // std::string
#include <vector>               // std::vector

#include "res.rh"               // Resource IDs

//------------------------------------------------------------
//
/// �f�o�b�O���j�^���n��
//
/// ����� Window Message ����M���邱�Ƃ� OutputDebugString �ŏo�͂�
/// �ꂽ��������擾����c�[�����n�����邽�߂̃V���O���g���N���X�ł��B
/// ���̂悤�ȃc�[�������݂��܂��������O�͖Y��܂����B
/// ���݂͎���̃c�[�����g�p���Ă܂��B
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
/// �V�X�e���̃G���[���b�Z�[�W���擾
//
/// inserts �͖���(���̂܂�)
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
/// ���\�[�X�̕�������擾
//

std::string LoadStr(UINT uID, HINSTANCE hInstance = 0)
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
//
/// ���W�X�g�� PreferLegacyPrintDialog �̒l�̓ǂݍ���
//

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

//------------------------------------------------------------
//
/// ���W�X�g�� PreferLegacyPrintDialog �̒l�̏�������
//

void WriteTheRegKey(bool legacy, HWND hwnd)
{
    if (ReadTheRegKey() == legacy)
        return;                 // �s�v
    //
    // moden/lecagy ���ꂼ��̃��W�X�g���t�@�C������ SHA-512 �n�b�V���l
    //
    // ���W�X�g���t�@�C���̉��ς�����ɂ��邽�߃n�b�V�����`�F�b�N����B
    //
    // Windows �R�}���h�v�����v�g�ł�
    //   certutil -hashfile <filename> SHA512
    // �Ōv�Z�ł���B
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
    // ������ legacy �̐^�U�ɂ��ǂ��炩��I��
    std::filesystem::path fname = moden_fname;
    const char* hash = moden_hash;
    if (legacy) {
        fname = legacy_fname;
        hash = legacy_hash;
    }
    TRACE("WriteTheRegKey: fname: " << fname);
    // �t�@�C���̑��݊m�F
    if (!std::filesystem::exists(fname))
        throw std::runtime_error("�t�@�C����������܂���: " + fname.string());
    // �n�b�V���m�F
    {
        boost::hash2::sha2_512 hasher;
        std::ifstream ifs{fname, std::ios::binary};
        if (!ifs)
            throw std::runtime_error("�t�@�C�����I�[�v���ł��܂���: " + fname.string());
        std::vector<char> buffer(512);
        while (ifs.read(buffer.data(), buffer.size()) || ifs.gcount())
            hasher.update(buffer.data(), ifs.gcount());
        auto digest = hasher.result();
        // �����Ȃ��Ƃ��Ĕ�r
        if (!std::equal(digest.begin(), digest.end(), reinterpret_cast<const unsigned char*>(hash)))
            throw std::runtime_error("�t�@�C�������Ă��܂�: " + fname.string());
    }
    // ���s
    auto hInst = ShellExecute(hwnd, "open", fname.string().c_str(), nullptr, nullptr, SW_NORMAL);
    if (auto ec = reinterpret_cast<INT_PTR>(hInst); ec <= 32) // �G���[
        throw std::runtime_error(GetErrorMessage(ec));
}

//------------------------------------------------------------
//
/// \class TMyClientDialog
//
/// �A�v���P�[�V�����t���[���̃N���C�A���g�_�C�A���O
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
    // ���W�X�g������l��ǂݍ��݁A����ɂ���ă��W�I�{�^���̃`�F�b�N��Ԃ�ݒ�
    //
    bool preferLegacyPrintDialog = ReadTheRegKey();
    CheckDlgButton(IDC_LEGACY, preferLegacyPrintDialog);
    CheckDlgButton(IDC_MODERN, !preferLegacyPrintDialog);
};

void TMyClientDialog::CloseWindow(int retVal)
{
    TRACE("TMyClientDialog::CloseWindow|retVal=" << retVal);
    if (retVal == IDOK) {
        try {
            if (!CanClose()) return;
            TRACE("TMyClientDialog::CloseWindow|Save");
            //
            // ���W�I�{�^���̃`�F�b�N��Ԃɂ��A���W�X�g���ɏ�������
            //
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
/// \class TMyFrameWindow
//
/// �A�v���P�[�V�����̃t���[���E�C���h�E
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
/// \class TMyApp
//
/// �A�v���P�[�V�����N���X
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
/// OWL �A�v���\�V�����̃G���g���|�C���g
//

int OwlMain(int argc, TCHAR** argv)
{
    std::string title = "(title not loaded)";
    int result = -1;
    try {
#if !defined(NDEBUG)                      // �f�o�b�O���[�h
        TDebugMonitorStarter::Instance(); // �f�o�b�O���j�^���n��
#endif    
        title = LoadStr(IDS_APPNAME);
        TMyApp app(title.c_str());
        result = app.Run();
        TRACE("OwlMain|result=" << result);
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
