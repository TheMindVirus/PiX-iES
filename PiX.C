#include "pix.h"

int main()
{
    platform_setup();
    uart_setup(115200);
    uart_write("\n\n");
    uart_write(PIX_WELCOME_MESSAGE);
    uart_write("\n\n");

    zero_packet(100);
    mbox_get_framebuffer();
    print_packet(0x90 / 4);
    uart_write("MONITOR_FRAMEBUFFER: ");
    uart_print((VALUE)Platform.MONITOR_FRAMEBUFFER);
    uart_write("\nMONITOR_PITCH_SPACE: ");
    uart_print((VALUE)Platform.MONITOR_PITCH_SPACE);
    uart_write("\n\n");
   
    for (ADDRESS y = 0; y < 1080; ++y)
    {
        for (ADDRESS x = 0; x < 1920; ++x)
        {
            drawPixel_ARM64(x, y, 0x0000AA);
        }
    }

    mmio_write(0x2010010C, 0x5A00107F); //PM_GRAFX
    mmio_write(0x2000A00C, 0x5A000000); //ASB_V3D_M_CTRL
    mmio_write(0x2000A008, 0x5A000000); //ASB_V3D_S_CTRL

    for (ADDRESS i = 0x20C00000; i < 0x20C01000; i += 4)
    {
        uart_write("[MMIO]: Address: ");
        uart_print((VALUE)i);
        uart_write(" | Value: ");
        VALUE value = mmio_read(i);
        uart_print((VALUE)value);
        uart_write(" | ");
        char ascii[4] = { (value >> 24) & 0xFF, (value >> 16) & 0xFF, (value >> 8) & 0xFF,  value & 0xFF };
        if ((ascii[3] >= 32) && (ascii[3] < 127)) { uart_write_byte(ascii[3]); }
        if ((ascii[2] >= 32) && (ascii[2] < 127)) { uart_write_byte(ascii[2]); }
        if ((ascii[1] >= 32) && (ascii[1] < 127)) { uart_write_byte(ascii[1]); }
        if ((ascii[0] >= 32) && (ascii[0] < 127)) { uart_write_byte(ascii[0]); }
        uart_write("\n");
    }

    for (ADDRESS i = 0x20100000; i < 0x20100114; i += 4)
    {
        uart_write("[MMIO]: Address: ");
        uart_print((VALUE)i);
        uart_write(" | Value: ");
        VALUE value = mmio_read(i);
        uart_print((VALUE)value);
        uart_write(" | ");
        char ascii[4] = { (value >> 24) & 0xFF, (value >> 16) & 0xFF, (value >> 8) & 0xFF,  value & 0xFF };
        if ((ascii[3] >= 32) && (ascii[3] < 127)) { uart_write_byte(ascii[3]); }
        if ((ascii[2] >= 32) && (ascii[2] < 127)) { uart_write_byte(ascii[2]); }
        if ((ascii[1] >= 32) && (ascii[1] < 127)) { uart_write_byte(ascii[1]); }
        if ((ascii[0] >= 32) && (ascii[0] < 127)) { uart_write_byte(ascii[0]); }
        uart_write("\n");
    }
    
    for (ADDRESS i = 0x2000A000; i < 0x2000A024; i += 4)
    {
        uart_write("[MMIO]: Address: ");
        uart_print((VALUE)i);
        uart_write(" | Value: ");
        VALUE value = mmio_read(i);
        uart_print((VALUE)value);
        uart_write(" | ");
        char ascii[4] = { (value >> 24) & 0xFF, (value >> 16) & 0xFF, (value >> 8) & 0xFF,  value & 0xFF };
        if ((ascii[3] >= 32) && (ascii[3] < 127)) { uart_write_byte(ascii[3]); }
        if ((ascii[2] >= 32) && (ascii[2] < 127)) { uart_write_byte(ascii[2]); }
        if ((ascii[1] >= 32) && (ascii[1] < 127)) { uart_write_byte(ascii[1]); }
        if ((ascii[0] >= 32) && (ascii[0] < 127)) { uart_write_byte(ascii[0]); }
        uart_write("\n");
    }

    while (1) { uart_loop(); }
    return 0;
}