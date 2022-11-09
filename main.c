//#include <stdio.h> //This goes nowhere by default
//#include <stdarg.h> //For varying number of params
#include <stdbool.h> //Only required for true/false

#include <pspkernel.h>
//#include <pspdebug.h>
#include <pspctrl.h>

//Potentially Missing Headers?//

//#include <stdlib.h>
//#include <pspuser.h>
//#include <pspstartup.h>

//#include <pspmoduleinfo.h>

//#include <pspdisplay.h>

////////////////////////////////

//#define log(...) pspDebugScreenPrintf(__VA_ARGS__);

PSP_MODULE_INFO("EBOOT", 0, 1, 0);

//extern int module_start;
//extern struct SceModuleInfo module_info;
//static const unsigned int __syslib_exports[4] __attribute__((section(".rodata.sceResident"))) =
//{
//    0xD632ACDB, 0xF01D73A7, (unsigned int) &module_start, (unsigned int) &module_info,
//};

//const

//int module_start();
int event_update(SceSize srgs, void* argp);
int event_exit(int arg1, int arg2, void* common);

//void setup();
//void loop();

//void setup()
//{
    //log("Hello World!");
//}

//void loop()
//{
    //log("[TEST]");
    //sleep(1000);
    //sceKernelSleepThreadCB();
//}

int PSP_DISPLAY_SET_MODE(int mode, int width, int height)
{
    //asm("li $v0, 0x48BA73F0; syscall;");
    //asm("li $v0, 0x0E20F177; syscall;");
}

int PSP_DISPLAY_SET_FBUF(void* topaddr, int bufferwidth, int pixelformat, int sync)
{
    //asm("li $v0, 0x08CC0000; syscall;");
}

int main2()
{
    //printf("/s\n", "<<<PSP3000>>>");

    //int threadId = sceKernelCreateThread("update_thread", event_update, 0x11, 0xFA0, 0, 0);
    //int threadId = PSP_KERNEL_CREATE_THREAD("update_thread", event_update, 0x11, 0xFA0, 0, 0);
    //if (threadId >= 0) { sceKernelStartThread(threadId, 0, 0); }
    //pspDebugScreenInit();
    //PSP_DISPLAY_SET_MODE(0, 480, 272);
    //PSP_DISPLAY_SET_FBUF((void*)0x04000000, 512, 3, 1);
    //sceCtrlSetSamplingCycle(0);
    //sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    //struct SceCtrlData controls; //loop() can't see this...
    //setup();

    volatile uint32_t* framebuffer = (volatile uint32_t*)0x04000000;
    for (int y = 0; y < 272; ++y)
    {
        for (int x = 0; x < 480; ++x)
        {
            framebuffer[(y * 0x01E0) + x] = 0xFFAAAAAA;
        }
    }

    bool running = false;
    while (running)
    {
        //sceCtrlReadBufferPositive(&controls, 1);
        //if (controls.Buttons & PSP_CTRL_CIRCLE) { running = false; } //log("X key pressed\n"); }
        //loop();
        //sceKernelExitGame();
    }
    //sceKernelExitGame();
    return 0; //If it reaches here it will turn off the PSP instead...
}

int event_update(SceSize args, void* argp)
{
    //int eventId = sceKernelCreateCallback("Exit Callback", event_exit, NULL);
    //sceKernelRegisterExitCallback(eventId);
    //sceKernelSleepThreadCB();
    return 0;
}

int event_exit(int arg1, int arg2, void* common)
{
    //sceKernelExitGame();
    return 0;
}

//int module_start() { return main(); } //This can be changed in the linker file
//int(*__executable_start)() = main;

