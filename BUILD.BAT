@echo on

rem No Comment. Except Maybe This One.
call ".\env.bat"

%PREFIX%-gcc %FLAGS% -c ".\boot.S" -o ".\%BUILD%\BOOT.o"
%PREFIX%-gcc %FLAGS% -c ".\PiX.c" -o ".\%BUILD%\PiX.o"
%PREFIX%-gcc %FLAGS% -c ".\iES.c" -o ".\%BUILD%\iES.o"
%PREFIX%-ld -nostdlib -nostartfiles ".\%BUILD%\boot.o" ".\%BUILD%\iES.o" ".\%BUILD%\PiX.o" -T ".\link.ld" -o ".\%BUILD%\PiX.elf"
%PREFIX%-objcopy -O binary ".\%BUILD%\PiX.elf" ".\%BUILD%\PiX.img"
copy ".\%BUILD%\PiX.img" ".\%BUILD%\PiX.iES"

pause