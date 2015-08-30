echo search
ipconfig >%3
set flag=0
find /i "%2" %3 && set flag=1
del %3 /q /f
if /I %flag% equ "1" echo success
goto end


:end
