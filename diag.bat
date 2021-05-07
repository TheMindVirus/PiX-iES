@echo on
rem [pix.ies]
set PATH=%PATH%;%CD%\xcompiler\bin
aarch64-none-elf-objdump -D kernel8.elf
pause