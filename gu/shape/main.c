//https://pspdev.github.io/

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>

PSP_MODULE_INFO("shape", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 272

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

char list[0x20000] __attribute__((aligned(64)));

void initGu()
{
    sceGuInit();

    //glBegin();
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)0x88000, BUFFER_WIDTH);
    sceGuDepthBuffer((void*)0x110000, BUFFER_WIDTH);

    //glViewport();
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    //glDepth();
    sceGuDepthRange(65535, 0);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST);

    //glEnd();
    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

void endGu()
{
    //glTerminate();
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

void startFrame()
{
    //glClear();
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0xFFFFFFFF);
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame()
{
    //glSync();
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

typedef struct
{
    unsigned short u, v;
    short x, y, z;
}   Vertex, *pVertex;

void drawRect(float x, float y, float w, float h)
{
    pVertex vertices = (pVertex)sceGuGetMemory(2 * sizeof(Vertex));
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[1].x = y + w;
    vertices[1].y = x + h;

    //glColor();
    sceGuColor(0xFF0000FF);
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
}

int main()
{
    initGu();
    int running = 1;
    while(running)
    {
        startFrame();
        drawRect(32, 32, 64, 64);
        endFrame();
    }
    return 0;
}
