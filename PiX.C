#include "PiX.H"

void main()
{
    platform_setup();
    uart_setup(115200);
    uart_write(PIX_WELCOME_MESSAGE);
    uart_write("\n\n");

    gpio_setup(Platform.HEADPHONES_LEFT, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.HEADPHONES_RIGHT, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_X1, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_X2, GPIO_OUTPUT, GPIO_PULL_NONE);

    for (ADDRESS i = 0; i < 3; ++i)
    {
        uart_write("[AUDS]: LEFT\n");
        gpio_write(Platform.HEADPHONES_LEFT);
        gpio_reset(Platform.HEADPHONES_RIGHT);
        gpio_write(Platform.DBUG_CARD_X1);
        gpio_reset(Platform.DBUG_CARD_X2);
        time_wait(ONE_SECOND);

        uart_write("[AUDS]: RIGHT\n");
        gpio_reset(Platform.HEADPHONES_LEFT);
        gpio_write(Platform.HEADPHONES_RIGHT);
        gpio_reset(Platform.DBUG_CARD_X1);
        gpio_write(Platform.DBUG_CARD_X2);
        time_wait(ONE_SECOND);
    }
    gpio_reset(Platform.HEADPHONES_LEFT);
    gpio_reset(Platform.HEADPHONES_RIGHT);
    gpio_reset(Platform.DBUG_CARD_X1);
    gpio_reset(Platform.DBUG_CARD_X2);

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

    print_memory();
    
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

    uart_write("[TIME]: ");
    uart_print(time_read());
    uart_write("\n");

    gpio_setup(Platform.DBUG_CARD_X1, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_A1, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_B1, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_C1, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_D1, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_E1, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_F1, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_G1, GPIO_OUTPUT, GPIO_PULL_NONE);
    
    gpio_setup(Platform.DBUG_CARD_X2, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_A2, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_B2, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_C2, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_D2, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_E2, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_F2, GPIO_OUTPUT, GPIO_PULL_NONE);
    gpio_setup(Platform.DBUG_CARD_G2, GPIO_OUTPUT, GPIO_PULL_NONE);

    gpio_write(Platform.DBUG_CARD_X1); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_A1); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_B1); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_C1); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_D1); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_E1); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_F1); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_G1); time_wait(ONE_TENTH);

    gpio_write(Platform.DBUG_CARD_X2); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_A2); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_B2); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_C2); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_D2); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_E2); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_F2); time_wait(ONE_TENTH);
    gpio_write(Platform.DBUG_CARD_G2); time_wait(ONE_TENTH);

    gpio_reset(Platform.DBUG_CARD_X1); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_A1); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_B1); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_C1); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_D1); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_E1); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_F1); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_G1); time_wait(ONE_TENTH);

    gpio_reset(Platform.DBUG_CARD_X2); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_A2); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_B2); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_C2); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_D2); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_E2); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_F2); time_wait(ONE_TENTH);
    gpio_reset(Platform.DBUG_CARD_G2); time_wait(ONE_TENTH);

    for (GO8BIT i = 0x0; i <= 0xF; ++i)
    {
        post_code((i << 4) + i);
        time_wait(ONE_TENTH);
    }

    post_code(0xAD); //Ready To Boot

    while (true)
    {
        uart_loop();
        gpio_write(Platform.DBUG_CARD_X1);
        gpio_reset(Platform.DBUG_CARD_X2);
        time_wait(ONE_TENTH);
        gpio_write(Platform.DBUG_CARD_X2);
        gpio_reset(Platform.DBUG_CARD_X1);
        time_wait(ONE_TENTH);
    }
}