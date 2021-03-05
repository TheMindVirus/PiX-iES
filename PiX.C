#include "PiX.H"

void main()
{
    platform_setup();
    gpio_pull(21, GPIO_PULL_UP);
    gpio_alt(21, GPIO_OUTPUT);
    gpio_set(21);
    uart_setup(115200);
    uart_write(PIX_WELCOME_MESSAGE);

    mbox_get_framebuffer();
    uart_write("\nMONITOR_FRAMEBUFFER: ");
    uart_print((VALUE)Platform.MONITOR_FRAMEBUFFER);
    uart_write("\nMONITOR_PITCH_SPACE: ");
    uart_print((VALUE)Platform.MONITOR_PITCH_SPACE);
    uart_write("\n");

    mbox_get_mon_info();
    uart_write("\nMONITOR_DISPLAY_ID[0]: ");
    uart_print((VALUE)Platform.MONITOR_DISPLAY_ID[0]);
    uart_write("\nMONITOR_NUM_DISPLAYS: ");
    uart_print((VALUE)Platform.MONITOR_NUM_DISPLAYS);
    uart_write("\nMONITOR_DISPLAY_ID[1]: ");
    uart_print((VALUE)Platform.MONITOR_DISPLAY_ID[1]);
    uart_write("\n");
    
    mbox_get_framebuffer2();
    uart_write("\nMONITOR_FRAMEBUFFER2: ");
    uart_print((VALUE)Platform.MONITOR_FRAMEBUFFER2);
    uart_write("\nMONITOR_PITCH_SPACE2: ");
    uart_print((VALUE)Platform.MONITOR_PITCH_SPACE2);
    uart_write("\n");
    
    gpio_clear(21);
    for (ADDRESS y = 0; y < 1080; ++y)
    {
        for (ADDRESS x = 0; x < 1920; ++x)
        {
            drawPixel_ARM64(x, y, 0x0000AA);
        }
    }
	
    for (ADDRESS y = 0; y < 1080; ++y)
    {
        for (ADDRESS x = 0; x < 1920; ++x)
        {
            drawPixel2_ARM64(x, y, 0xAA0000);
        }
    }

    while (1)
    {
        gpio_set(21);
        uart_loop();
        gpio_clear(21);
    }
}                                                                                                                                                                                                                                                                                                                                                                                                    