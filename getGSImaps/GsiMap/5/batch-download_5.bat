@echo off
setlocal enabledelayedexpansion

REM x の範囲: 0 ～ 31
for /L %%x in (0,1,31) do (
    REM x の値でサブディレクトリ作成
    if not exist %%x (
        mkdir %%x
    )

    REM y の範囲: 0 ～ 31
    for /L %%y in (0,1,31) do (
        REM ダウンロード先URLと保存先パス
        set "url=https://cyberjapandata.gsi.go.jp/xyz/std/5/%%x/%%y.png"
        set "outfile=%%x\%%y.png"

        REM curlで画像をダウンロード（エラーでも継続）
        if not exist !outfile! (
            curl -s -f !url! -o !outfile!
            if errorlevel 1 (
                echo [ERROR] %%x/%%y.png のダウンロードに失敗しました。
            ) else (
                echo [OK] %%x/%%y.png を保存しました。
            )
        )
    )
)

echo 完了しました。
pause
