【ソフト名　】 LegacyPrintDlg - 印刷ダイアログの新旧切替
【バージョン】 1.0.0
【動作確認　】 Windows 11
【種別　　　】 フリーソフトウェア
【転載・条件】 GPLv3 に従い可
【著作者　　】 neige68
【連絡先　　】 Github Issues https://github.com/neige68/LegacyPrintDlg/issues


【概要】

Windows 11 (22H2 以降) では、Win32 アプリに対して新しい印刷画面が導入
されたことで、印刷設定が正しく反映されないなどの問題が発生することがあ
ります。

当アプリ LegacyPrintDlg は、印刷時の画面を旧型に切り替えることで、アプ
リ側が指定した印刷設定が確実に反映されるようにするためのツールです。


【動作環境】

- Windows 11 バージョン 22H2 以降


【主な機能】

- Win32 アプリの印刷ダイアログの新旧切り替え


【同梱ファイル】

- LegacyPrintDlg.exe                実行ファイル
- UnifiedPrintDialog_Legacy.reg     レガシー(旧)ダイアログを使用するレジストリ設定
- UnifiedPrintDialog_Modern.reg     モダン(新)ダイアログを使用するレジストリ設定
- Readme.txt                        本ドキュメント
- License.rtf                       ライセンス

* レジストリ設定ファイル(.reg)はプログラムでハッシュをチェックしてます
  ので、改変できません。


【インストール手順】

- LegacyPrintDlg.msi を実行してインストール


【使い方】

1. スタートメニューまたはショートカットから当アプリ LegacyPrintDlg を
   起動

2. 「印刷ダイアログの種類」で「レガシー(旧)」または「モダン(新)」を選
   択し OK ボタンをクリック

3. 「ユーザーアカウント制御」ダイアログの「デバイスに変更を加えること
   を許可しますか?」で「はい」をクリック

4. 「レジストリ エディター」警告メッセージボックスの「続行しますか?」
   で「はい」をクリック

5. 「レジストリ エディター」情報メッセージボックスで OK をクリック

6. Win32 アプリの印刷処理にて選択したダイアログが表示される


【アンインストール】

- Windows スタートメニュー → 設定 → アプリ から LegacyPrintDlg を選択してアンインストール


【ライセンス】

- 本ソフトは GPLv3 ライセンスの下で配布できます。
  https://www.gnu.org/licenses/gpl-3.0.html


【サポート】

- GitHub Issues にて対応  
  https://github.com/neige68/LegacyPrintDlg/issues


【参考リンク】

- Restore the Legacy Print Dialog in Windows 11 22H2
  https://www.winhelponline.com/blog/restore-legacy-print-dialog-windows-11/


【更新履歴】

- Ver.1.0.0 (2025/07/11) 初版リリース
