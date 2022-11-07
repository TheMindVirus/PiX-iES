//#include <stdio.h> //This goes nowhere by default
//#include <stdarg.h> //For varying number of params
//#include <stdbool.h> //Only required for true/false

//#include <pspkernel.h>
//#include <pspdebug.h>
//#include <pspctrl.h>

//Potentially Missing Headers?//

//#include <stdlib.h>
//#include <pspuser.h>
//#include <pspstartup.h>

////////////////////////////////

//#define log(...) pspDebugScreenPrintf(__VA_ARGS__);

//PSP_MODULE_INFO("EBOOT", 0, 1, 0);

//int module_start();
//int event(SceSize srgs, void* argp);
//int event_exit(int arg1, int arg2, void* common);
/*
void setup();
void loop();

void setup()
{
    //log("Hello World!");
}

void loop()
{
    //log("[TEST]");
    //sleep(1000);
}
*/
int main()
{
    //printf("/s\n", "<<<PSP3000>>>");

    //int threadId = sceKernelCreateThread("update_thread", event, 0x11, 0xFA0, 0, 0);
    //if (threadId >= 0) { sceKernelStartThread(threadId, 0, 0); }
    //pspDebugScreenInit();
    //sceCtrlSetSamplingCycle(0);
    //sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    //struct SceCtrlData controls; //loop() can't see this...
    //setup();

    while (1) //true)
    {
        //sceCtrlReadBufferPositive(&controls, 1);
        //if (controls.Buttons & PSP_CTRL_CIRCLE) { log("X key pressed\n"); }
        //loop();
    }
    return 0;
}
/*
int event(SceSize args, void* argp)
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

int module_start() { return main(); } //This can be changed in the linker file
*/
