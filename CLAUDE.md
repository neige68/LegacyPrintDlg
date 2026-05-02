# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## プロジェクト概要

Windows 11 (22H2 以降) で印刷ダイアログを旧型(レガシー)と新型(モダン)に切り替えるツール。
レジストリキー `HKCU\Software\Microsoft\Print\UnifiedPrintDialog\PreferLegacyPrintDialog` を
`.reg` ファイル経由で書き換える。切り替えには権限の昇格が必要。

## ビルド

**前提環境（Windows）:**
- Visual Studio 2022 (VC14.3)：環境変数 `VC143` に `vcvarsall.bat` のディレクトリを設定
- CMake 3.28 以上
- Boost 1.88 以上（`find_package` で検索）

**ビルド実行（WSL2 から）:**
```bash
# build ディレクトリを作成して cmake + msbuild を実行
powershell.exe -NoProfile -Command "Set-Location 'C:\Users\neige\github\LegacyPrintDlg'; cmd /c build.bat"
```

`build.bat` のオプション:
- 引数なし: Debug + Release 両方ビルド
- `deb` / `d`: Debug のみ
- `rel` / `r`: Release のみ
- `rebuild` / `re`: `build/` ディレクトリを削除して cmake からやり直し
- `cm`: cmake のみ再実行

**出力先:**
- `build\src\debug\LegacyPrintDlg.exe`
- `build\src\release\LegacyPrintDlg.exe`

**実行（WSL2 から）:**
```bash
# Debug 版を実行
powershell.exe -NoProfile -Command "Set-Location 'C:\Users\neige\github\LegacyPrintDlg'; exec.bat deb"
# Release 版を実行
powershell.exe -NoProfile -Command "Set-Location 'C:\Users\neige\github\LegacyPrintDlg'; exec.bat rel"
```

## インストーラビルド

WiX Toolset v3（`candle.exe` / `light.exe`）を使用。Release ビルド後に `installer/` で実行:
```bat
cd installer
make
```
出力: `build\installer\LegacyPrintDlg.msi`

## Lint

```bash
cpplint --output=emacs src/main.cpp src/pch.h src/version.h
```

設定は `src/cpplint.cfg`。除外フィルタ多数（`build/header_guard`、`whitespace/braces` 等）、行長 115 文字。

## コードアーキテクチャ

コードはほぼ `src/main.cpp` 1 ファイルに集約されている。Win32 API を直接使用した
フラットな構造で、OWL などの UI フレームワーク依存はない。

**主要ダイアログプロシージャ:**
- `MainDlgProc`: メインダイアログ（IDD_MAIN）。起動時にレジストリを読んでラジオボタンを設定、OK 時に書き込み
- `AboutDlgProc`: 「アプリについて」ダイアログ（IDD_ABOUT）。バージョン表示と GitHub リンク

**主要関数:**
- `ReadTheRegKey()`: レジストリから `PreferLegacyPrintDialog` を読み取る（Win32 API 直接使用）
- `WriteTheRegKey(bool legacy, HWND hwnd)`: `.reg` ファイルの SHA-512 ハッシュを検証してから `ShellExecute` で実行
- `wWinMain()`: エントリポイント。`DialogBoxParam(IDD_MAIN)` でメインダイアログを表示

**レジストリファイル:**
- `assets/UnifiedPrintDialog_Legacy.reg`: レガシー設定（`PreferLegacyPrintDialog=1`）
- `assets/UnifiedPrintDialog_Modern.reg`: モダン設定（`PreferLegacyPrintDialog=0`）
- ハッシュ値は `main.cpp` の `WriteTheRegKey()` 内にハードコードされている。**ファイルを変更したらハッシュ値も更新が必要**

**ビルド設定:**
- C++20、スタティックリンク（`/MT` / `/MTd`）
- プリコンパイルヘッダ: `pch.h` / `pch.cpp`（Windows SDK ヘッダのみ）
- リソース: `src/res.rc`（アイコン・文字列）+ `src/resource.rc`（バージョン情報）
- マニフェストは `src/LegacyPrintDlg.manifest` を埋め込み（`requestedExecutionLevel=requireAdministrator`）
- `comctl32.lib` リンク（InitCommonControlsEx / SysLink コントロール用）

**バージョン管理:**
- `src/version.h` の `VER_PRODUCTVERSION` / `VER_PRODUCTVERSION_STR` を更新する
- CMake が `PROJECT_CURRENT_YEAR` を自動設定し、著作権年を `2025-YYYY` 形式にする
