@echo on

rem ...or maybe also this one...
call "..\env.bat"

copy "..\%BUILD%\PiX.o" "..\%BUILD%\PiX.debug.o"
%PREFIX%-objcopy --rename-section .text=/folder/boot "..\%BUILD%\PiX.debug.o"
%PREFIX%-objcopy --set-section-flags .ussr=alloc "..\%BUILD%\PiX.debug.o"

%PREFIX%-objdump -h "..\%BUILD%\PiX.debug.o"

copy "..\%BUILD%\PiX.elf" "..\%BUILD%\PiX.debug.elf"
%PREFIX%-objcopy --add-section watchdogs.png=watchdogs.png "..\%BUILD%\PiX.debug.elf"
%PREFIX%-objcopy --change-section-address watchdogs.png=0x200000000 "..\%BUILD%\PiX.debug.elf"

%PREFIX%-gcc -dM -E "..\PiX.c" > "defines_full.txt"
%PREFIX%-gcc -undef -dM -E "..\PiX.c" > "defines_min.txt"

%PREFIX%-objdump -h "..\%BUILD%\PiX.debug.elf" > "headers.txt"
%PREFIX%-objdump -d "..\%BUILD%\PiX.debug.elf" > "disasm.txt"

rem %PREFIX%-objdump -Q --help=target "M_target.txt"
rem %PREFIX%-objdump -Q --help=warnings > "W_warnings.txt"
rem %PREFIX%-objdump -Q --help=optimizers > "f_optimizers.txt"
rem %PREFIX%-objdump -Q --help=undocumented > "undocumented.txt"

%PREFIX%-objcopy -O binary "..\%BUILD%\PiX.debug.elf" "..\%BUILD%\PiX.debug.img"
copy "..\%BUILD%\PiX.debug.img" "..\%BUILD%\PiX.debug.iES"

pause