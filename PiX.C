#include "pix.h"

#define sleep() { for (ADDRESS i = 0; i < 0xFFFFFFF; ++i) { int a = 0; a = a; } } //use timer instead!

int main()
{
    platform_setup();
    gpio_pull(21, GPIO_PULL_UP);
    gpio_alt(21, GPIO_OUTPUT);
    gpio_set(21);
    uart_setup(115200);
    uart_write(PIX_WELCOME_MESSAGE);
    uart_write("\n\n");

    ADDRESS pwm1_base = 0xFE20C800;
    for (ADDRESS i = pwm1_base; i <= (pwm1_base + 0x24); i+=4)
    {
        uart_write("[PWM1]: ");
        uart_print(i);
        uart_write(" = ");
        uart_print(mmio_read(i));
        uart_write("\n");
    }
    uart_write("\n");

    mmio_write((pwm1_base + 0x08), 0x80000707);

    for (ADDRESS i = pwm1_base; i <= (pwm1_base + 0x24); i+=4)
    {
        uart_write("[PWM1]: ");
        uart_print(i);
        uart_write(" = ");
        uart_print(mmio_read(i));
        uart_write("\n");
    }
    uart_write("\n");

    //mini audio test
    gpio_pull(40, GPIO_PULL_NONE);
    gpio_alt(40, GPIO_OUTPUT);
    gpio_pull(41, GPIO_PULL_NONE);
    gpio_alt(41, GPIO_OUTPUT);
    while (1)
    {
        gpio_set(40);
        gpio_clear(41);
        uart_write("sleep 1");
        sleep();
        gpio_clear(40);
        gpio_set(41);
        uart_write("sleep 0");
        sleep();
    }

    zero_packet(100);
    mbox_get_framebuffer();
    print_packet(0x90 / 4);
    uart_write("MONITOR_FRAMEBUFFER: ");
    uart_print((VALUE)Platform.MONITOR_FRAMEBUFFER);
    uart_write("\nMONITOR_PITCH_SPACE: ");
    uart_print((VALUE)Platform.MONITOR_PITCH_SPACE);
    uart_write("\n\n");

    zero_packet(100);
    mbox_get_mon_info();
    print_packet(0x4C / 4);
    uart_write("MONITOR_DISPLAY_ID[0]: ");
    uart_print((VALUE)Platform.MONITOR_DISPLAY_ID[0]);
    uart_write("\nMONITOR_NUM_DISPLAYS: ");
    uart_print((VALUE)Platform.MONITOR_NUM_DISPLAYS);
    uart_write("\nMONITOR_DISPLAY_ID[1]: ");
    uart_print((VALUE)Platform.MONITOR_DISPLAY_ID[1]);
    uart_write("\n\n");
  
    zero_packet(100);  
    mbox_get_framebuffer2();
    print_packet(0x90 / 4);
    uart_write("MONITOR_FRAMEBUFFER2: ");
    uart_print((VALUE)Platform.MONITOR_FRAMEBUFFER2);
    uart_write("\nMONITOR_PITCH_SPACE2: ");
    uart_print((VALUE)Platform.MONITOR_PITCH_SPACE2);
    uart_write("\n\n");

    mbox_get_edid();
    uart_write("BEGIN_EDID\n");
    for (ADDRESS i = 0; i < Platform.EDIDsize; ++i)
    {
        uart_write("EDID[");
        uart_print((VALUE)i);
        uart_write("] = ");
        uart_print((VALUE)Platform.EDID[i]);
        uart_write("\n");
    }
    uart_write("\nEND_EDID\n\n");

/*
    zero_packet(100);
    uart_write("mbox_get_vsync() = ");
    uart_print((VALUE)mbox_get_vsync());
    uart_write("\n\n");
    print_packet(0x1C / 4);
    
    zero_packet(100);
    mbox_set_vsync(0xFFFFFFFF);
    print_packet(0x1C / 4);
    
    zero_packet(100);
    uart_write("mbox_set_vsync() = ");
    uart_print((VALUE)mbox_get_vsync());
    uart_write("\n\n");
    print_packet(0x1C / 4);
*/

    print_memory();
   
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
        //gpio_set(21);
        //uart_loop();
        //gpio_clear(21);
    }
    return 0;
}