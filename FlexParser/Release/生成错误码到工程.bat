@echo off

title 错误码生成工具
color 6
set Var=%1
if "%1" NEQ "" (
cd %1 && %Var:~1,2%
)

:head
cls
echo **********************错误码生成管理****************************
echo 1.计费
echo 2.杰大师
echo 3.电竞
echo 4.打开当前脚本目录
echo 0.退出

echo 请输入命令：
set /p flag=

if /i %flag% == 1 (
start cmd /C call .\Script.\yjf主版本.bat %cd%\Script
goto head
)^
else if /i %flag% == 2 (
start cmd /C call .\Script.\yjf杰大师.bat %cd\Script
goto head
)^
else if /i %flag% == 3 (
rem start cmd /C "cd .\dj2\ && call 电竞酒店.bat" %cd\Script
goto head
)^
else if /i %flag% == 4 (
start %CD%
goto head
)^
else if /i %flag% == 0 (
exit(0)
)