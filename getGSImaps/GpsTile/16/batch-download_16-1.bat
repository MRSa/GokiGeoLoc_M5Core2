@echo off
setlocal enabledelayedexpansion

REM �J�E���^�[������
set success=0
set error=0

REM x �͈̔�: 58140 �` 59520
for /L %%x in (58140,1,59520) do (
    REM x �̒l�ŃT�u�f�B���N�g���쐬
    if not exist %%x (
        mkdir %%x
    )

    REM y �͈̔�: 23430 �` 24500
    for /L %%y in (23430,1,24500) do (
        REM �_�E�����[�h��URL�ƕۑ���p�X
        set "url=https://cyberjapandata.gsi.go.jp/xyz/std/16/%%x/%%y.png"
        set "outfile=%%x\%%y.png"
        if not exist !outfile! (
            REM powershell -Command "Start-Sleep -Milliseconds 250"
            REM curl�ŉ摜���_�E�����[�h�i�G���[�ł��p���j
            curl -s -f !url! -o !outfile!
            if errorlevel 1 (
                echo [ERROR] %%x/%%y.png �̃_�E�����[�h�Ɏ��s���܂����B
                set /a error+=1
            ) else (
                echo [OK] %%x/%%y.png ��ۑ����܂����B
                set /a success+=1     
            )
        )
    )
)

echo �_�E�����[�h���������܂����B
echo ��������: %success%
echo �G���[����: %error%
pause
