@echo off
setlocal enabledelayedexpansion

REM x �͈̔�: 0 �` 31
for /L %%x in (0,1,31) do (
    REM x �̒l�ŃT�u�f�B���N�g���쐬
    if not exist %%x (
        mkdir %%x
    )

    REM y �͈̔�: 0 �` 31
    for /L %%y in (0,1,31) do (
        REM �_�E�����[�h��URL�ƕۑ���p�X
        set "url=https://cyberjapandata.gsi.go.jp/xyz/std/5/%%x/%%y.png"
        set "outfile=%%x\%%y.png"

        REM curl�ŉ摜���_�E�����[�h�i�G���[�ł��p���j
        if not exist !outfile! (
            curl -s -f !url! -o !outfile!
            if errorlevel 1 (
                echo [ERROR] %%x/%%y.png �̃_�E�����[�h�Ɏ��s���܂����B
            ) else (
                echo [OK] %%x/%%y.png ��ۑ����܂����B
            )
        )
    )
)

echo �������܂����B
pause
