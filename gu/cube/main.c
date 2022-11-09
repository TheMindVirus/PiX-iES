//https://github.com/pspdev/pspsdk/tree/master/src/samples/gu/cube

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <pspgu.h>
#include <pspgum.h>

#include "callbacks.h"
#include "vram.h"

PSP_MODULE_INFO("Cube Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

static unsigned int __attribute__((aligned(16))) list[262144];
extern unsigned char logo_start[];

struct Vertex
{
    float u, v;
    unsigned int color;
    float x, y, z;
};

struct Vertex __attribute__((aligned(16))) vertices[12 * 3] =
{
    { 0, 0, 0xFF7F0000, -1, -1,  1 }, //1
    { 1, 0, 0xFF7F0000, -1,  1,  1 },
    { 1, 1, 0xFF7F0000,  1,  1,  1 },

    { 0, 0, 0xFF7F0000, -1, -1,  1 }, //2
    { 1, 1, 0xFF7F0000,  1,  1,  1 },
    { 0, 1, 0xFF7F0000,  1, -1,  1 },

    { 0, 0, 0xFF7F0000, -1, -1, -1 }, //3
    { 1, 0, 0xFF7F0000,  1, -1, -1 },
    { 1, 1, 0xFF7F0000,  1,  1, -1 },

    { 0, 0, 0xFF7F0000, -1, -1, -1 }, //4
    { 1, 1, 0xFF7F0000,  1,  1, -1 },
    { 0, 1, 0xFF7F0000, -1,  1, -1 },

    { 0, 0, 0xFF007F00,  1, -1, -1 }, //5
    { 1, 0, 0xFF007F00,  1, -1,  1 },
    { 1, 1, 0xFF007F00,  1,  1,  1 },

    { 0, 0, 0xFF007F00,  1, -1, -1 }, //6
    { 1, 1, 0xFF007F00,  1,  1,  1 },
    { 0, 1, 0xFF007F00,  1,  1, -1 },

    { 0, 0, 0xFF007F00, -1, -1, -1 }, //7
    { 1, 0, 0xFF007F00, -1,  1, -1 },
    { 1, 1, 0xFF007F00, -1,  1,  1 },

    { 0, 0, 0xFF007F00, -1, -1, -1 }, //8
    { 1, 1, 0xFF007F00, -1,  1,  1 },
    { 0, 1, 0xFF007F00, -1, -1,  1 },

    { 0, 0, 0xFF00007F, -1,  1, -1 }, //9
    { 1, 0, 0xFF00007F,  1,  1, -1 },
    { 1, 1, 0xFF00007F,  1,  1,  1 },

    { 0, 0, 0xFF00007F, -1,  1, -1 }, //10
    { 1, 1, 0xFF00007F,  1,  1,  1 },
    { 0, 1, 0xFF00007F, -1,  1,  1 },

    { 0, 0, 0xFF00007F, -1, -1, -1 }, //11
    { 1, 0, 0xFF00007F, -1, -1,  1 },
    { 1, 1, 0xFF00007F,  1, -1,  1 },

    { 0, 0, 0xFF00007F, -1, -1, -1 }, //12
    { 1, 1, 0xFF00007F,  1, -1,  1 },
    { 0, 1, 0xFF00007F,  1, -1, -1 },
};

#define BUFFER_WIDTH (512)
#define BUFFER_HEIGHT (272)
#define SCREEN_WIDTH (480)
#define SCREEN_HEIGHT (272)

int main(int argc, char* argv[])
{
    setupCallbacks();
    void* fbp0 = getStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
    void* fbp1 = getStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
    void* zbp = getStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_4444);

    sceGuInit();
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, fbp0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, fbp1, BUFFER_WIDTH);
    sceGuDepthBuffer(zbp, BUFFER_WIDTH); //should be Buffer Depth?

    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuDepthRange(65535, 0);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST);

    sceGuFrontFace(GU_CW);
    sceGuShadeModel(GU_SMOOTH);
    sceGuEnable(GU_CULL_FACE);
    sceGuEnable(GU_TEXTURE_2D); //sampler2D...sampler3D?...
    sceGuEnable(GU_CLIP_PLANES);

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);

    int val = 0;
    while(running())
    {
        sceGuStart(GU_DIRECT, list);
        sceGuClearColor(0xFF554433);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        sceGumMatrixMode(GU_PROJECTION); //UNITY_MATRIX_MVP, or PVM in this case
        sceGumLoadIdentity();
        sceGumPerspective(75.0f, 16.0f / 9.0f, 0.5f, 1000.0f); //16:9? A SONY Specialty?
        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();

        { //Scope Required?
            ScePspFVector3 pos = { 0, 0, -2.5f };
            ScePspFVector3 rot = { val * 0.79f * (GU_PI / 180.0f), val * 0.98f * (GU_PI / 180.0f), val * 1.32f * (GU_PI / 180.0f) };
            sceGumTranslate(&pos);
            sceGumRotateXYZ(&rot);
        }

        sceGuTexMode(GU_PSM_4444, 0, 0, 0);
        sceGuTexImage(0, 64, 64, 64, logo_start);
        sceGuTexFunc(GU_TFX_ADD, GU_TCC_RGB);
        sceGuTexEnvColor(0xFFFF00);
        sceGuTexFilter(GU_LINEAR, GU_LINEAR);
        sceGuTexScale(1.0f, 1.0f);
        sceGuTexOffset(0.0f, 0.0f);
        sceGuAmbientColor(0xFFFFFFFF);

        sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 12 * 3, 0, vertices);

        sceGuFinish();
        sceGuSync(0, 0);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();

        val++;
    }

    sceGuTerm();
    sceKernelExitGame();
    return 0;
}
