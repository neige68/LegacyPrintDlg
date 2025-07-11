# LegacyPrintDlg - Windows 11 22H2 以降の Win32 アプリからの印刷ダイアログの新旧切替

Windows 11 (22H2 以降) では、Win32 アプリに対して新しい印刷ダイアログが導入されたことで、
印刷設定が正しく反映されないなどの問題が発生することがあります。

当アプリ LegacyPrintDlg は、印刷時のダイアログを旧型に切り替えることで、アプ
リ側が指定した印刷設定が確実に反映されるようにするためのツールです。

レジストリ操作でも行えますが、それを GUI 操作で行えるようにしたアプリです。

スクリーンショット<br>
![スクリーン ショット](image/LegacyPrintDlg.png)

レガシー(旧型)ダイアログの例<br>
![レガシーダイアログの例](image/Legacy.png)

モダン(新型)ダイアログの例<br>
![モダンダイアログの例](image/Modern.png)

切り替えには権限の昇格が必要となります。
システムにより警告メッセージも表示されますが、続行して頂かないと切り替えられません。
利用者の責任でご利用ください。

2つのレジストリ(.reg)ファイルを同梱してあり、それを実行するだけのプログラムです。

[OWLNext7](https://sourceforge.net/projects/owlnext/) を使用した 64 ビット版の単一 .exe ファイルでできています。いまのところ日本語版だけです。

ライセンスは GPLv3 に従います。

レジストリファイル(.reg)はハッシュをチェックしてますので、改変しないでください。

## Link

Original Article

[Restore the Legacy Print Dialog in Windows 11 22H2](https://www.winhelponline.com/blog/restore-legacy-print-dialog-windows-11/)
