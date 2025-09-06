@echo off
setlocal enabledelayedexpansion

REM カウンター初期化
set success=0
set error=0

REM x の範囲: 58140 ～ 59520
for /L %%x in (58140,1,59520) do (
    REM x の値でサブディレクトリ作成
    if not exist %%x (
        mkdir %%x
    )

    REM y の範囲: 23430 ～ 24500
    for /L %%y in (23430,1,24500) do (
        REM ダウンロード先URLと保存先パス
        set "url=https://cyberjapandata.gsi.go.jp/xyz/std/16/%%x/%%y.png"
        set "outfile=%%x\%%y.png"
        if not exist !outfile! (
            REM powershell -Command "Start-Sleep -Milliseconds 250"
            REM curlで画像をダウンロード（エラーでも継続）
            curl -s -f !url! -o !outfile!
            if errorlevel 1 (
                echo [ERROR] %%x/%%y.png のダウンロードに失敗しました。
                set /a error+=1
            ) else (
                echo [OK] %%x/%%y.png を保存しました。
                set /a success+=1     
            )
        )
    )
)

echo ダウンロードが完了しました。
echo 成功件数: %success%
echo エラー件数: %error%
pause
