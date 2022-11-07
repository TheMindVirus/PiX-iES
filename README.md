# pspdev-nw
pspsdk non-working example of PSP3000 MIPS32 Toolchain being rejected by PPSSPP Emulator \
for generating an invalid ELF Header

https://github.com/pspdev/pspsdk
https://github.com/pspdev/psptoolchain
https://github.com/hrydgard/ppsspp

Introducing a new custom version of the elf2pbp utility! \
It uses the pack-pbp and unpack-pbp tool scripts from the pspsdk \
and simplifies the process of packaging a new homebrew PSP Game Bundle. \

![pspdev](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/pspdev.png)

The tool is similar to elf2uf2 for RP2040-based platforms, \
but the format and architecture is different.

A UF2 bootloader for the Sony PSP3000, much like it's relative the Pico, would make it an ideal \
IoT device as Stackless Python has been missing some bindings to physical hardware resources \
such as GU (rip-off of GL GPU), FPU, VFPU and several on-board video codec accelerators.

![memoryview](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/memoryview.png)

A lot of the steps in the Makefile are actually unnecessary \
and has been extrapolated from the build process of PiX-iES which has fewer errors.
While PiX-iES completely strips the harshly limiting ELF Header from the build, \
the PSP3000 actually uses it to falsely allocate the wrong size of memory for the input file.

![diff](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/diff.png)

It is also important to note that the size of the missing ELF Header GNU Section \
is not 32 bytes, not 54 bytes, not 64 bytes but 84 bytes (0x54) from the start of the file. \
If a start address in the link.ld script is specified correctly as 0, this will be subtly \
edited to 4096 (0x1000) because the size of the ELF Header file is unknown to psp-ld.

![archive](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/archive.png)

In the pursuit of making the smallest possible EBOOT.PBP file, it was found that an empty \
PBP file consists of the text PBP followed by null characters and some brackets as pictured:

![pbpstructure](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/pbpstructure.png)

A PBP package which contains a data.psp (actually stripped kernel.elf) with only 1 line \
might become valid in future releases. A kernel containing only 1 bit - whether the device \
is switched on or not - would be a possibility but redundant complexity prevents it from \
becoming a reality in a lot of edge-cases.

![startup](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/startup.png)

This gave rise to the idea of a "half-bit kernel", one that has been half-eaten \
or one that uses 0.5 bits (0.1 in binary). This is a quantum group state where \
the state of the bit depends on the state of itself in 2 separate locations/contexts \
e.g. it depends on both an event that happens locally and on the other side of the world.

![chests](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/chests.png)

https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
https://github.com/hrydgard/ppsspp/blob/ceb4186d0ffc26091bb3fc25a1e71bce15b8902d/Core/HLE/sceKernelModule.cpp#L1265 

Another reason why the PSP fails to boot certain ELF files is several airport-security-style checks \
for the "magic pointer", which in this case points to "\x7fELF" but can be "~PSP" or "~SCE".
PiX requires and should require no such checks for compulsory option modules for successful boot \
but option cards would be accepted as a valid use-case in the context of a home Edge Server (edge-case).

![eboot](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/eboot.png)

The eventual goal would be to run content generated for the new Web 3.0 Standards, \
especially Unity WebGL Player in a custom version of Mozilly Nightly with their API's, \
but recent firmware updates have rendered Wi-Fi useless and Unity lacks PSP Build Support.

Sample Game for Porting: https://youtu.be/yzesMQdo0nw
Source: https://github.com/TheMindVirus/unity-dev/tree/main/WebGLCueStack
![RKN](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/RKN.jpg)

This branch has been developed in the hope that it will be useful for developing highly-optimised \
and graphics-compliant edge-case app and game packages for the PSP3000, MIPS32 and other platforms. \
Also included are some python examples for Stackless Python 2.5 for PSP (with limited hardware support).

![sprites](https://github.com/TheMindVirus/PiX-iES/blob/pspdev-nw/screenshots/sprites.png)