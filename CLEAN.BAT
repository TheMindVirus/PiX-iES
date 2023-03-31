@echo on

rem ...and this one...
call ".\env.bat"

del ".\%BUILD%\*.o" ".\%BUILD%\*.elf" ".\%BUILD%\*.img" ".\%BUILD%\*.iES"

pause