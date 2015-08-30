@echo off
if /i "%1" equ "del" (
call %~dp0\ip_del.bat %1 %2 %3
goto end
)

if /i "%1" equ "add" (
call %~dp0\ip_add.bat %1 %2 %3 %4 %5
goto end
)

if /i "%1" equ "search" (
call %~dp0\ip_search.bat %1 %2 %3
goto end
)


:end
