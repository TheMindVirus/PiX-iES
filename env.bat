@echo off

echo "============================="
echo "-- (V) -- PiX-iES -- +2023 --"
echo "============================="
echo "[] ||\\ * \\//    * === // []"
echo "[] ||// |  \\  == | === \\ []"
echo "[] ||   | //\\    | === // []"
echo "============================="
echo "EdgeCase Code-Assembly-System"
echo "============================="

set PATH=%PATH%;%~DP0%\compiler\bin
set PREFIX=aarch64-none-elf
set FLAGS=-Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -g
set BUILD=build

@echo on