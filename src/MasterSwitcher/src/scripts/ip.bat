@echo off

set cmd=%1
set ethernet=%3
set ip=%2
set netmask=%4
set gateway=%5
set broadcast=%6

echo %cmd%.
echo %ethernet%.
echo %ip%.
echo %netmask%.
echo %gateway%.
echo %broadcast%.

if %cmd% equ "add" {
echo add
netsh interface ipv4 add address name=%ethernet% %ip% %netmask% %gateway%
ipconfig | findstr /i %ip% && set add_flag=1 || set add_flag=0
if %add_flag% equ 1 (
echo success
) else (
echo fail
)
goto end
}
if %cmd% equ "del" {
echo del
echo netsh interface ipv4 delete address name=%ethernet% %ip%
ipconfig | findstr /i %ip% && set del_flag=0 || set del_flag=1
if %del_flag% equ 1 (
echo success
) else (
echo fail
)
goto end
}
if %cmd% equ "search" {
echo search
ipconfig | findstr /i %ip% && set del_flag=1 || set del_flag=0
if %del_flag% equ 1 (
echo success
) else (
echo fail
)
goto end
}

:end

