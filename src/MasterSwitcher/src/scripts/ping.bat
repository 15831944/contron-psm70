@echo off
ping -n %2 %1 >%3
set flag=0
find /i "TTL=" %3 && set flag=1
del %3 /q /f
if /I %flag% equ "1" echo success
goto end


:end


