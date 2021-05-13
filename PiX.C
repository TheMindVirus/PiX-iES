#include "pix.h"

void main()
{
    platform_setup();
    //uart_setup(115200);
    uart_write(PIX_WELCOME_MESSAGE);

    const char* test1 = "53";
    int dtest1 = atoi(test1);
    uart_write("[INFO]: TEST1: ");
    uart_print(dtest1);
    uart_write("\n");

    double test2_a = 1.0;
    double test2_b = 2.0;
    double test2_c = test2_a + test2_b;
    uart_write("[INFO]: TEST2: ");
    if (test2_c > 2.0) { uart_write("TRUE"); }
    else { uart_write("FALSE"); }
    uart_write("\n");

    double test3 = pow(2.0, 6);
    uart_write("[INFO]: TEST3: ARM");
    uart_eval((VALUE)test3);
    uart_write("\n");
/*
    int argc = 3;
    const char* argv[] = {"8", "7", "10"};

    int mb = 0;
    int result = 0;
    int loops = 0;
    int freq = 0;
    int log2_N = 0;
    int jobs = 0;
    int N = 0;

    unsigned t[2];
    double tsq[2];

    struct GPU_FFT_COMPLEX* base;
    struct GPU_FFT* fft;

    log2_N = argc > 1? atoi(argv[1]) : 12; // 8 <= log2_N <= 22
    jobs   = argc > 2? atoi(argv[2]) : 1;  // transforms per batch
    loops  = argc > 3? atoi(argv[3]) : 1;  // test repetitions

    if ((argc < 2) || (jobs < 1) || (loops < 1))
    {
        uart_write("[ASRT]: (argc < 2) || (jobs < 1) || (loops < 1)\n");
        while(1);
    }

    N = 1 << log2_N; // FFT length
    result = gpu_fft_prepare(mb, log2_N, GPU_FFT_REV, jobs, &fft); // call once

    switch (result)
    {
        case (-1): { uart_write("[WARN]: Unable to enable V3D\n"); while(1); } break;
        case (-2): { uart_write("[ASRT]: 8 <= log2_N <= 22\n");    while(1); } break;
        case (-3): { uart_write("[WARN]: Out of Memory\n");        while(1); } break;
        case (-4): { uart_write("[WARN]: MMIO Failure.\n");        while(1); } break;
        case (-5): { uart_write("[WARN]: Platform Failure.\n");    while(1); } break;
        default: break;
    }

    for (int k = 0; k < loops; ++k)
    {
        for (int j = 0; j < jobs; ++j)
        {
            base = fft->in + j * fft->step; // input buffer
            for (int i = 0; i < N; ++i) { base[i].re = base[i].im = 0; }
            freq = (j + 1) & (N / 2 - 1);
            base[freq].re += base[(N - freq) & (N - 1)].re = 0.5;
        }

        gpu_fft_execute(fft); // call one or many times

        tsq[0] = tsq[1] = 0;
        for (int j = 0; j < jobs; ++j)
        {
            base = fft->out + j * fft->step; // output buffer
            freq = (j + 1) & (N / 2 - 1);
            for (int i = 0; i < N; ++i)
            {
                double re = cos(2 * GPU_FFT_PI * freq * i / N);
                tsq[0] += pow(re, 2);
                tsq[1] += pow(re - base[i].re, 2) + pow(base[i].im, 2);
            }
        }

        //printf("rel_rms_err = %0.2g, usecs = %d, k = %d\n",
        //    sqrt(tsq[1] / tsq[0]), (t[1] - t[0]) / jobs, k);
        uart_write("[INFO]: REL_RMS_ERR: ");
        uart_print((int)sqrt(tsq[1] / tsq[0]));
        uart_write("\n");

        uart_write("[INFO]: K: ");
        uart_print(k);
        uart_write("\n");
    }

    gpu_fft_release(fft); // Videocore memory lost if not freed !

*/
    uart_write("\n[TEST]: COMPLETE\n");
    return;
}