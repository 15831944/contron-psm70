@echo off
echo add
netsh interface ipv4 add address %3 %2 %4 %5
goto end

:end
