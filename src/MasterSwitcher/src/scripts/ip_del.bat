@echo off
echo del
netsh interface ipv4 delete address %3 addr=%2
goto end


:end
