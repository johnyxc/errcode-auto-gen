@echo off

title ���������ɹ���
color 6
set Var=%1
if "%1" NEQ "" (
cd %1 && %Var:~1,2%
)

:head
cls
echo **********************���������ɹ���****************************
echo 1.�Ʒ�
echo 2.�ܴ�ʦ
echo 3.�羺
echo 4.�򿪵�ǰ�ű�Ŀ¼
echo 0.�˳�

echo ���������
set /p flag=

if /i %flag% == 1 (
start cmd /C call .\Script.\yjf���汾.bat %cd%\Script
goto head
)^
else if /i %flag% == 2 (
start cmd /C call .\Script.\yjf�ܴ�ʦ.bat %cd\Script
goto head
)^
else if /i %flag% == 3 (
rem start cmd /C "cd .\dj2\ && call �羺�Ƶ�.bat" %cd\Script
goto head
)^
else if /i %flag% == 4 (
start %CD%
goto head
)^
else if /i %flag% == 0 (
exit(0)
)