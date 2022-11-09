
PSPINC = 
PSPINC += -I ./include/kernel
PSPINC += -I ./include/user
PSPINC += -I ./include/base
PSPINC += -I ./include/modinfo
PSPINC += -I ./include/display
PSPINC += -I ./include/debug
PSPINC += -I ./include/ctrl
#PSPINC += -I ./include/#

PSPLIB = 
PSPLIB += -L ./lib/kernel -lpspkernel
PSPLIB += -L ./lib/user -lpspuser
PSPLIB += -L ./lib/base
PSPLIB += -L ./lib/modinfo -lpspmodinfo
PSPLIB += -L ./lib/display -lpspdisplay -lpspdisplay_driver
PSPLIB += -L ./lib/debug -lpspdebug -lpspdebugkb -lpspgdb -lpspgdb_kernel -lpspgdb_user
PSPLIB += -L ./lib/ctrl -lpspctrl
#PSPLIB += -L ./lib/#

#PSPINC += -I ./include/gu
#PSPLIB += -L ./lib/gu -lpspgu

#PSPINC += -I ./include/fpu
#PSPLIB += -L ./lib/fpu -lpspfpu

#PSPINC += -I ./include/vfpu
#PSPLIB += -L ./lib/vfpu -lpspvfpu

#PSPINC += -I ./include/mp3
#PSPLIB += -L ./lib/mp3 -lpspmp3

#PSPINC += -I ./include/mpeg
#PSPLIB += -L ./lib/mpeg -lpspjpeg -lpspmpeg -lpspmpegbase -lpspmpegbase_driver

#PSPINC += -I ./include/video
#PSPLIB += -L ./lib/video -lpspvideocodec

PSPLIB += -L ./lib/libcglue
PSPLIB += -L ./lib/libpthreadglue
PSPLIB += -L ./lib/ge -lpspge -lpspge_driver

PSPLNK = 
PSPLNK = -T link.ld
#PSPLNK += -T ./lib/base/linkfile.prx.in

PSPDEF = 
#PSPDEF += -D F_sceDisplay_0001 # SetMode
#PSPDEF += -D F_sceDisplay_0006 # SetFbuf
PSPDEF += -D F_sceDisplay_driver_0003 # SetMode
PSPDEF += -D F_sceDisplay_driver_0012 # SetFbuf
PSPDEF += -D F_sceGe_user_0002 # EdramGetAddr
PSPDEF += -D F_SysclibForKernel_0024 # strlen

all:
	export PSPOUT="/mnt/c/users/alastair/documents/PPSSPP/memstick/PSP/GAME/RKN" && \
	export PSPDEV="/usr/local/pspdev" && \
	export PSPBIN="$$PSPDEV/bin" && \
	export PSPLIB="$$PSPDEV/lib" && \
	export PSPSDK="./elf2pbp" && \
	export PSPGCC="$$PSPLIB/gcc/psp/11.2.0" && \
	export PATH="$$PATH:$$PSPBIN:./elf2pbp" && \
	psp-gcc -c main.c -o main.o $(PSPINC) && \
	#psp-as include/base/pspimport.s -o pspimport.o && \
	psp-gcc -c include/display/sceDisplay_driver.S -o sceDisplay_driver.o $(PSPINC) $(PSPDEF) && \
	psp-gcc -c include/kernel/SysclibForKernel.S -o SysclibForKernel.o $(PSPINC) $(PSPDEF) && \
	psp-ld -nmagic main.o sceDisplay_driver.o SysclibForKernel.o -o kernel.elf $(PSPLNK) $(PSPLIB) && \
	#psp-strip -s kernel.elf && \
	psp-objdump -x kernel.elf && \
	psp-readelf -a kernel.elf && \
	elf2pbp kernel.elf EBOOT.PBP && \
	cp ./EBOOT.PBP $$PSPOUT && \
	echo "Done!"
clean:
	rm -f ./kernel.elf
	rm -f ./EBOOT.PBP
	rm -f ./main.o
