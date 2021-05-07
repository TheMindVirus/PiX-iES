#include "pix.hxx"

void main()
{
    platform_setup();
    uart_write(PIX_WELCOME_MESSAGE);

    unsigned long eln = 0;
    asm volatile ("mrs %[eln], CurrentEL;" : [eln] "=rm"(eln));
    uart_write("[INFO]: Current Exception Level: ");
    uart_print((VALUE)(eln >> 2));
    uart_write("\n");
    uart_write("[INFO]: Baud Reg: ");
    uart_print((VALUE)((Platform.UART_CLOCK / (115200 * 8)) - 1));
    uart_write("\n\n");

    unsigned long long a = 0;
    unsigned long long b = 0;
    unsigned long long c = 0;
    asm volatile (
	"mrs %[a], sctlr_el2;"
        "mov %[b], #1;"
        "orr %[a], %[a], %[b];"
        "msr sctlr_el2, %[a];"
        "mrs %[c], sctlr_el2;"
        : [a] "=rm"(a), [b] "=rm"(b), [c] "=rm"(c)
    ); //Enable MMU - Stage 1 Address Translation
    uart_write("a[H] = ");
    uart_print((VALUE)(a >> 32));
    uart_write("\n");
    uart_write("a[L] = ");
    uart_print((VALUE)(a >>  0));
    uart_write("\n");
    uart_write("b[H] = ");
    uart_print((VALUE)(b >> 32));
    uart_write("\n");
    uart_write("b[L] = ");
    uart_print((VALUE)(b >>  0));
    uart_write("\n");
    uart_write("c[H] = ");
    uart_print((VALUE)(c >> 32));
    uart_write("\n");
    uart_write("c[L] = ");
    uart_print((VALUE)(c >>  0));
    uart_write("\n");

    uart_write("[CALL]: LDUR/STUR\n");
    asm volatile (
        "ldr     x0, =0x1018;"
        "ldr     x1, =0x2018;"
        "str     xzr, [x0], #0;"
        "str     xzr, [x1], #0;"
        "ldur    w1, [x1, #7];"
        "stur    w1, [x0, #7];"
        "sub     w1, w1, w1;"
        "sub     x0, x0, x0;"
        "sub     x1, x1, x1;"
    );
    
    uart_write("\n[TEST]: COMPLETE\n");
    return;
}