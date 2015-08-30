@echo off
echo %time%
set ping_flag=0
ping -n %2 %1 | findstr /i TTL= && set ping_flag=1 || set ping_flag=0
rem echo %time%
rem echo flag=%ping_flag%
if %ping_flag% equ 1 (
echo success
) else (
echo fail
)

