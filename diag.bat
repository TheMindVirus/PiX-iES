@echo on
set PATH=%PATH%;%CD%\xcompiler
arm-none-eabi-objdump -D myos.elf
pause