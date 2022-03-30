#include "iES.H"

PLATFORM Platform;

void platform_setup(void)
{
    Platform.HEXMAP = "0123456789ABCDEF";

    Platform.PERI_BASE = 0x00000000FE000000;

    Platform.GPIO_BASE               = Platform.PERI_BASE + 0x0000000000200000;
    Platform.GPIO_GPFSEL0            = Platform.GPIO_BASE + 0x0000000000000000;
    Platform.GPIO_GPSET0             = Platform.GPIO_BASE + 0x000000000000001C;
    Platform.GPIO_GPCLR0             = Platform.GPIO_BASE + 0x0000000000000028;
    Platform.GPIO_GPLEV0             = Platform.GPIO_BASE + 0x0000000000000034;
    Platform.GPIO_PUP_PDN_CNTRL_REG0 = Platform.GPIO_BASE + 0x00000000000000E4;

    Platform.AUX_BASE        = Platform.PERI_BASE + 0x0000000000215000;
    Platform.AUX_IRQ         = Platform.AUX_BASE  + 0x0000000000000000;
    Platform.AUX_ENABLES     = Platform.AUX_BASE  + 0x0000000000000004;
    Platform.AUX_MU_IO_REG   = Platform.AUX_BASE  + 0x0000000000000040;
    Platform.AUX_MU_IER_REG  = Platform.AUX_BASE  + 0x0000000000000044;
    Platform.AUX_MU_IIR_REG  = Platform.AUX_BASE  + 0x0000000000000048;
    Platform.AUX_MU_LCR_REG  = Platform.AUX_BASE  + 0x000000000000004C;
    Platform.AUX_MU_MCR_REG  = Platform.AUX_BASE  + 0x0000000000000050;
    Platform.AUX_MU_LSR_REG  = Platform.AUX_BASE  + 0x0000000000000054;
    Platform.AUX_MU_CNTL_REG = Platform.AUX_BASE  + 0x0000000000000060;
    Platform.AUX_MU_BAUD_REG = Platform.AUX_BASE  + 0x0000000000000068;

    Platform.UART_TX      = 14;
    Platform.UART_RX      = 15;
    Platform.UART_CLOCK   = 500000000;
    Platform.UART_COUNTER = 0;

    Platform.MBOX_BASE   = Platform.PERI_BASE + 0x000000000000B880;
    Platform.MBOX_READ   = Platform.MBOX_BASE + 0x0000000000000000;
    Platform.MBOX_STATUS = Platform.MBOX_BASE + 0x0000000000000018;
    Platform.MBOX_WRITE  = Platform.MBOX_BASE + 0x0000000000000020;

    Platform.MONITOR_FRAMEBUFFER = 0x00000000FE2FE000; //Temporary, will be rewritten...
    Platform.MONITOR_PITCH_SPACE = 0x0000000000000000;

    Platform.MONITOR_NUM_DISPLAYS = 0;
    for (ADDRESS i = 0; i < 5; ++i) { Platform.MONITOR_DISPLAY_ID[i] = 0; }
    
    Platform.MONITOR_FRAMEBUFFER2 = 0x00000000FE2FE000; //Temporary, will be rewritten...
    Platform.MONITOR_PITCH_SPACE2 = 0x0000000000000000;

    Platform.HEADPHONES_LEFT  = 41;
    Platform.HEADPHONES_RIGHT = 40;

    Platform.DBUG_CARD_X1 = 16;
    Platform.DBUG_CARD_A1 = 17;
    Platform.DBUG_CARD_B1 = 4;
    Platform.DBUG_CARD_C1 = 25;
    Platform.DBUG_CARD_D1 = 24;
    Platform.DBUG_CARD_E1 = 23;
    Platform.DBUG_CARD_F1 = 22;
    Platform.DBUG_CARD_G1 = 27;

    Platform.DBUG_CARD_X2 = 26;
    Platform.DBUG_CARD_A2 = 5;
    Platform.DBUG_CARD_B2 = 6;
    Platform.DBUG_CARD_C2 = 19;
    Platform.DBUG_CARD_D2 = 21;
    Platform.DBUG_CARD_E2 = 20;
    Platform.DBUG_CARD_F2 = 12;
    Platform.DBUG_CARD_G2 = 13;

    Platform.TIMER_BASE            = Platform.PERI_BASE  + 0x0000000000003000;
    Platform.TIMER_COUNTER_LOW     = Platform.TIMER_BASE + 0x0000000000000004;
    Platform.TIMER_COUNTER_HIGH    = Platform.TIMER_BASE + 0x0000000000000008;
    Platform.TIMER_CHECKPOINT_LAST = 0;

    for (ADDRESS i = 0; i < CLOCK_LIMIT; ++i)
    {
        Platform.CLOCK[i].ID = i;
        Platform.CLOCK[i].STATE = 0;
        Platform.CLOCK[i].RATE = 0;
        Platform.CLOCK[i].MEASURED = 0;
        Platform.CLOCK[i].MAX = 0;
        Platform.CLOCK[i].MIN = 0;
    }
    Platform.CLOCK_TURBO = 0;

    Platform.CLOCK[CLOCK_ID_ROOT].MAX      =          0; //What's going on here?
    Platform.CLOCK[CLOCK_ID_EMMC].MAX      =  250000000; //These have been pulled from
    Platform.CLOCK[CLOCK_ID_UART].MAX      = 1000000000; //the mailbox directly
    Platform.CLOCK[CLOCK_ID_ARM].MAX       = 1500000000; //but not at the same time
    Platform.CLOCK[CLOCK_ID_CORE].MAX      =  500000000; //as the measured clock rates
    Platform.CLOCK[CLOCK_ID_V3D].MAX       =  500000000;
    Platform.CLOCK[CLOCK_ID_H264].MAX      =  500000000;
    Platform.CLOCK[CLOCK_ID_ISP].MAX       =  500000000;
    Platform.CLOCK[CLOCK_ID_SDRAM].MAX     =  400000000;
    Platform.CLOCK[CLOCK_ID_PIXEL].MAX     = 2400000000;
    Platform.CLOCK[CLOCK_ID_PWM].MAX       =  500000000;
    Platform.CLOCK[CLOCK_ID_HEVC].MAX      =  500000000;
    Platform.CLOCK[CLOCK_ID_EMMC2].MAX     =  500000000;
    Platform.CLOCK[CLOCK_ID_M2MC].MAX      =  600000000;
    Platform.CLOCK[CLOCK_ID_PIXEL_BVB].MAX =  300000000;
}

void gpio_setup(ADDRESS pin, ADDRESS alt, ADDRESS pull)
{
    ADDRESS bank = Platform.GPIO_GPFSEL0 + (0x4 * (pin / 10));
    ADDRESS remainder = pin % 10;
    VALUE value = mmio_read(bank);
    value &= ~(0b111 << (3 * remainder));
    value |= (alt << (3 * remainder));
    mmio_write(bank, value);

    bank = Platform.GPIO_PUP_PDN_CNTRL_REG0 + (0x4 * (pin / 16));
    remainder = pin % 16;
    value = mmio_read(bank);
    value &= ~(0b11 << (2 * remainder));
    value |= (pull << (2 * remainder));
    mmio_write(bank, value);
}

void uart_setup(VALUE baud)
{
    gpio_setup(Platform.UART_TX, GPIO_ALT5, GPIO_PULL_NONE);
    gpio_setup(Platform.UART_RX, GPIO_ALT5, GPIO_PULL_NONE);

    mmio_write(Platform.AUX_ENABLES, 1);
    mmio_write(Platform.AUX_MU_CNTL_REG, 0);
    mmio_write(Platform.AUX_MU_LCR_REG, 3); //8-bit Mode (should be 1 but reserved bit needs setting)
    mmio_write(Platform.AUX_MU_IIR_REG, 0xC6); //Clear FIFO
    mmio_write(Platform.AUX_MU_BAUD_REG, ((Platform.UART_CLOCK / (baud * 8)) - 1)); //8x Oversampling
    mmio_write(Platform.AUX_MU_CNTL_REG, 3);
    for(int i = 0; i < MAX_SIZE; ++i) { uart_write_byte('\0'); }
}

LITERAL uart_read(ADDRESS length)
{
    for (ADDRESS i = 0; i < length; ++i) { Platform.UART_QUEUE[i] = uart_read_byte(); }
    return (LITERAL)Platform.UART_QUEUE;
}

void uart_write_byte(BYTE byte)
{
    while (!uart_idle());
    mmio_write(Platform.AUX_MU_IO_REG, (VALUE)byte);
}

void uart_write_buffer(BUFFER buffer)
{
    while (REFER buffer)
    {
        if ('\n' == REFER buffer) { uart_write_byte('\r'); }
        uart_write_byte(REFER buffer);
        ++buffer;
    }
}

void uart_loop(void)
{
    if (uart_peek() != 0)
    {
        BYTE byte = uart_read_byte();
        if (('\r' == byte)
        ||  ('\n' == byte))
        {
            Platform.UART_QUEUE[Platform.UART_COUNTER] = '\0';
            uart_write("\r\n");
            uart_write_buffer(Platform.UART_QUEUE);
            uart_write("\r\n");
            Platform.UART_COUNTER = 0;
        }
        else
        {
            Platform.UART_QUEUE[Platform.UART_COUNTER] = byte;
            uart_write_byte(byte);
            if (Platform.UART_COUNTER < (MAX_SIZE) - 1) { ++Platform.UART_COUNTER; }
        }
    }
}

void uart_print(ADDRESS value)
{
    uart_write("0x");
    for (ADDRESS i = sizeof(ADDRESS) * 8; i > 0; i -= 4)
    {
        uart_write_byte(Platform.HEXMAP[((value >> (i - 4)) & 0xF)]);
    }
}

void uart_eval(VALUE value)
{
    ADDRESS a = 1;
    VALUE v = 0;
    VALUE tmp = value;
    while (tmp > 0)
    {
        tmp = (int)(tmp / 10);
        v += 1;
        a *= 10;
    }
    v = (v == 0) ? 1 : v;
    for (VALUE i = 0; i < v; ++i)
    {
        a /= 10;
        uart_write_byte(Platform.HEXMAP[(VALUE)(value / a) % 10]);
    }
}

VALUE mbox_setup(BYTE channel)
{
    VALUE checked = 0;
    VALUE mail = ((((ADDRESS)Platform.Mailbox) &~ 0xF) | (channel & 0xF)); //0xF reserved for 4-bit channel
   
    while ((mbox_peek() & MBOX_FULL) != 0);
    mbox_write(mail);
    while (1)
    {
        while((mbox_peek() & MBOX_EMPTY) != 0);
        checked = mbox_read();
        //uart_print(checked);
        //uart_write("\n");
        if (mail == checked) { return MBOX_SUCCESS; }
    }
    return MBOX_FAILURE;
}

void mbox_get_framebuffer(void)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    ADDRESS b = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00048003; //MBOX_TAG_SETPHYHW
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 1080;       //Value

    Platform.Mailbox[i++] = 0x00048004; //MBOX_TAG_SETVIRTHW
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 1080;       //Value

    Platform.Mailbox[i++] = 0x00048009; //MBOX_TAG_SETVIRTOFF
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 0;          //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048005; //MBOX_TAG_SETDEPTH
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 0;          //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048006; //MBOX_TAG_SETXPLODR
    Platform.Mailbox[i++] = 4;          //Data Length (bytes)
    Platform.Mailbox[i++] = 1 * 4;      //Data Length
    Platform.Mailbox[i++] = 1;          //Value

    Platform.Mailbox[i++] = 0x00040001; //MBOX_TAG_GETFB
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
a=i;Platform.Mailbox[i++] = 4096;       //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00040008; //MBOX_TAG_GETPITCH
    Platform.Mailbox[i++] = 4;          //Data Length (bytes)
    Platform.Mailbox[i++] = 1 * 4;      //Data Length
b=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8))
    {
        Platform.MONITOR_FRAMEBUFFER = Platform.Mailbox[a] & 0x3FFFFFFF; //Translate from VC to ARM address
        Platform.MONITOR_PITCH_SPACE = Platform.Mailbox[b];
    }
}

void mbox_get_mon_info(void)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    ADDRESS b = 0;
    ADDRESS c = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00040013; //MBOX_TAG_GET_NUM_DISPLAYS
    Platform.Mailbox[i++] = 4;          //Data Length (bytes)
    Platform.Mailbox[i++] = 1 * 4;      //Data Length
a=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00040016; //MBOX_TAG_GET_DISPLAY_ID
    Platform.Mailbox[i++] = 4;          //Data Length (bytes)
    Platform.Mailbox[i++] = 1 * 4;      //Data Length
b=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048013; //MBOX_TAG_SET_DISPLAY_NUM
    Platform.Mailbox[i++] = 4;          //Data Length (bytes)
    Platform.Mailbox[i++] = 1 * 4;      //Data Length
    Platform.Mailbox[i++] = 1;          //Value //Change to 1 if it doesn't work

    Platform.Mailbox[i++] = 0x00040016; //MBOX_TAG_GET_DISPLAY_ID
    Platform.Mailbox[i++] = 4;          //Data Length (bytes)
    Platform.Mailbox[i++] = 1 * 4;      //Data Length
c=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8))
    {
        Platform.MONITOR_NUM_DISPLAYS = Platform.Mailbox[a];
        Platform.MONITOR_DISPLAY_ID[0] = Platform.Mailbox[b];
        Platform.MONITOR_DISPLAY_ID[1] = Platform.Mailbox[c];
    }
}

void mbox_get_framebuffer2(void)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    ADDRESS b = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00048003; //MBOX_TAG_SETPHYHW
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 1080;       //Value

    Platform.Mailbox[i++] = 0x00048004; //MBOX_TAG_SETVIRTHW
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 1080;       //Value

    Platform.Mailbox[i++] = 0x00048009; //MBOX_TAG_SETVIRTOFF
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048005; //MBOX_TAG_SETDEPTH
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 0;          //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048006; //MBOX_TAG_SETXPLODR
    Platform.Mailbox[i++] = 4;          //Data Length (bytes)
    Platform.Mailbox[i++] = 1 * 4;      //Data Length
    Platform.Mailbox[i++] = 1;          //Value

    Platform.Mailbox[i++] = 0x00040001; //MBOX_TAG_GETFB
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
a=i;Platform.Mailbox[i++] = 4096;       //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00040008; //MBOX_TAG_GETPITCH
    Platform.Mailbox[i++] = 4;          //Data Length (bytes)
    Platform.Mailbox[i++] = 1 * 4;      //Data Length
b=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8))
    {
        Platform.MONITOR_FRAMEBUFFER2 = Platform.Mailbox[a] & 0x3FFFFFFF; //Translate from VC to ARM address
        Platform.MONITOR_PITCH_SPACE2 = Platform.Mailbox[b];
    }
}

void print_memory(void)
{
    for (ADDRESS i = 0; i < 9; ++i)
    {
        uart_write("[MBOX]: MBOX[");
        uart_print((VALUE)i);
        uart_write("] | Address: ");
        uart_print(Platform.MBOX_BASE + (i * 4));
        uart_write(" | Value: ");
        uart_print((VALUE)(*(POINTER)(Platform.MBOX_BASE + (i * 4))));
        uart_write("\n");
    }
    uart_write("\n");
}

void print_packet(VALUE n)
{
    for (ADDRESS i = 0; i < n; ++i)
    {
        uart_write("[MBOX]: Platform.Mailbox[");
        uart_print((VALUE)i);
        uart_write("] = ");
        uart_print((VALUE)Platform.Mailbox[i]);
        uart_write("\n");
    }
    uart_write("\n");
}

void zero_packet(VALUE n)
{
    for (ADDRESS i = 0; i < n; ++i)
    {
        Platform.Mailbox[i] = 0;
    }
}

GO8BIT QCODE2DIGIT(GO8BIT Q, BOOLEAN DOT)
{
    GO8BIT D = 0x0;
         if (Q == 0x0) { D = 0x3F; }
    else if (Q == 0x1) { D = 0x06; }
    else if (Q == 0x2) { D = 0x5B; }
    else if (Q == 0x3) { D = 0x4F; }
    else if (Q == 0x4) { D = 0x66; }
    else if (Q == 0x5) { D = 0x6D; }
    else if (Q == 0x6) { D = 0x7D; }
    else if (Q == 0x7) { D = 0x07; }
    else if (Q == 0x8) { D = 0x7F; }
    else if (Q == 0x9) { D = 0x6F; }
    else if (Q == 0xA) { D = 0x77; }
    else if (Q == 0xB) { D = 0x7C; }
    else if (Q == 0xC) { D = 0x39; }
    else if (Q == 0xD) { D = 0x5E; }
    else if (Q == 0xE) { D = 0x79; }
    else if (Q == 0xF) { D = 0x71; }
    D = (D << 1) | (DOT ? 1 : 0);
    return D;
}

void post_code(GO8BIT qcode)
{
    GO8BIT digit1 = QCODE2DIGIT((qcode >> 4) & 0xF, true);
    GO8BIT digit2 = QCODE2DIGIT((qcode     ) & 0xF, true);
    
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

    if (((digit1     ) & 1) == 1) { gpio_write(Platform.DBUG_CARD_X1); } else { gpio_reset(Platform.DBUG_CARD_X1); }
    if (((digit1 >> 1) & 1) == 1) { gpio_write(Platform.DBUG_CARD_A1); } else { gpio_reset(Platform.DBUG_CARD_A1); }
    if (((digit1 >> 2) & 1) == 1) { gpio_write(Platform.DBUG_CARD_B1); } else { gpio_reset(Platform.DBUG_CARD_B1); }
    if (((digit1 >> 3) & 1) == 1) { gpio_write(Platform.DBUG_CARD_C1); } else { gpio_reset(Platform.DBUG_CARD_C1); }
    if (((digit1 >> 4) & 1) == 1) { gpio_write(Platform.DBUG_CARD_D1); } else { gpio_reset(Platform.DBUG_CARD_D1); }
    if (((digit1 >> 5) & 1) == 1) { gpio_write(Platform.DBUG_CARD_E1); } else { gpio_reset(Platform.DBUG_CARD_E1); }
    if (((digit1 >> 6) & 1) == 1) { gpio_write(Platform.DBUG_CARD_F1); } else { gpio_reset(Platform.DBUG_CARD_F1); }
    if (((digit1 >> 7) & 1) == 1) { gpio_write(Platform.DBUG_CARD_G1); } else { gpio_reset(Platform.DBUG_CARD_G1); }

    if (((digit2     ) & 1) == 1) { gpio_write(Platform.DBUG_CARD_X2); } else { gpio_reset(Platform.DBUG_CARD_X2); }
    if (((digit2 >> 1) & 1) == 1) { gpio_write(Platform.DBUG_CARD_A2); } else { gpio_reset(Platform.DBUG_CARD_A2); }
    if (((digit2 >> 2) & 1) == 1) { gpio_write(Platform.DBUG_CARD_B2); } else { gpio_reset(Platform.DBUG_CARD_B2); }
    if (((digit2 >> 3) & 1) == 1) { gpio_write(Platform.DBUG_CARD_C2); } else { gpio_reset(Platform.DBUG_CARD_C2); }
    if (((digit2 >> 4) & 1) == 1) { gpio_write(Platform.DBUG_CARD_D2); } else { gpio_reset(Platform.DBUG_CARD_D2); }
    if (((digit2 >> 5) & 1) == 1) { gpio_write(Platform.DBUG_CARD_E2); } else { gpio_reset(Platform.DBUG_CARD_E2); }
    if (((digit2 >> 6) & 1) == 1) { gpio_write(Platform.DBUG_CARD_F2); } else { gpio_reset(Platform.DBUG_CARD_F2); }
    if (((digit2 >> 7) & 1) == 1) { gpio_write(Platform.DBUG_CARD_G2); } else { gpio_reset(Platform.DBUG_CARD_G2); }
}

TIME time_read(void)
{
    return (mmio_read(Platform.TIMER_COUNTER_HIGH) << 8) + mmio_read(Platform.TIMER_COUNTER_LOW);
}

void time_write(TIME checkpoint)
{
    Platform.TIMER_CHECKPOINT_LAST = checkpoint;
}

BOOLEAN time_check(TIME offset)
{
    return (time_read() > (Platform.TIMER_CHECKPOINT_LAST + offset));
}

void time_wait(TIME ticks)
{
    time_write(time_read());
    while (true) { if (time_check(ticks)) { break; } }
}

void clock_setup(void)
{
    checkpoint(0);

    clock_update();
    checkpoint(1);

    uart_print_data("CLOCK_ROOT_MAX",      Platform.CLOCK[0x0].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_EMMC_MAX",      Platform.CLOCK[0x1].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_UART_MAX",      Platform.CLOCK[0x2].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_ARM_MAX",       Platform.CLOCK[0x3].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_CORE_MAX",      Platform.CLOCK[0x4].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_V3D_MAX",       Platform.CLOCK[0x5].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_H264_MAX",      Platform.CLOCK[0x6].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_ISP_MAX",       Platform.CLOCK[0x7].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_SDRAM_MAX",     Platform.CLOCK[0x8].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_PIXEL_MAX",     Platform.CLOCK[0x9].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_PWM_MAX",       Platform.CLOCK[0xA].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_HEVC_MAX",      Platform.CLOCK[0xB].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_EMMC2_MAX",     Platform.CLOCK[0xC].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_M2MC_MAX",      Platform.CLOCK[0xD].MAX, "Hz (32-bit)");
    uart_print_data("CLOCK_PIXEL_BVB_MAX", Platform.CLOCK[0xE].MAX, "Hz (32-bit)");

    uart_print_hex("CLOCK_TURBO", Platform.CLOCK_TURBO, 0);
    checkpoint(2);

    //clock_write(CLOCK_ID_ROOT,      CLOCK_STATE_ON_PRESENT,          0); //clock_write_turbo(0);
    //clock_write(CLOCK_ID_EMMC,      CLOCK_STATE_ON_PRESENT,  249987312); //These have been pulled from
    //clock_write(CLOCK_ID_UART,      CLOCK_STATE_ON_PRESENT,   48001464); //obtaining Platform.CLOCK[0xX].MEASURED
    //clock_write(CLOCK_ID_ARM,       CLOCK_STATE_ON_PRESENT,  600117184); //If the Mailbox read and write succeeds,
    //clock_write(CLOCK_ID_CORE,      CLOCK_STATE_ON_PRESENT,  500000992); //why not set it to maximum values instead?
    //clock_write(CLOCK_ID_V3D,       CLOCK_STATE_ON_PRESENT,  250000496); //Well at least V3D exists
    //clock_write(CLOCK_ID_H264,      CLOCK_STATE_ON_PRESENT,          0); //Video Encode is a bit dead
    //clock_write(CLOCK_ID_ISP,       CLOCK_STATE_ON_PRESENT,          0); //same with Image encode
    //clock_write(CLOCK_ID_SDRAM,     CLOCK_STATE_ON_PRESENT,          0); //and with SDRAM
    //clock_write(CLOCK_ID_PIXEL,     CLOCK_STATE_ON_PRESENT,   75001464); //Pixel Clock should be 2.4GHz instead, or 2.0GHz to be safe
    //clock_write(CLOCK_ID_PWM,       CLOCK_STATE_ON_PRESENT,          0); //PWM is also dead
    //clock_write(CLOCK_ID_HEVC,      CLOCK_STATE_ON_PRESENT,  250000496); //but there is HEVC Video
    //clock_write(CLOCK_ID_EMMC2,     CLOCK_STATE_ON_PRESENT,   99997560); //and EMMC2 memory
    //clock_write(CLOCK_ID_M2MC,      CLOCK_STATE_ON_PRESENT,  119997072); //and something undocumented called M2MC (memory to memory controller?)
    //clock_write(CLOCK_ID_PIXEL_BVB, CLOCK_STATE_ON_PRESENT,          0); //Pixel BVB is also dead
    checkpoint(3);

    clock_write(CLOCK_ID_PIXEL, CLOCK_STATE_ON_PRESENT, 2400000000); //2.4GHz Pixel Clock
    time_wait(TIME_ONE_SECOND * 3); //!!!
    clock_update();
    checkpoint(4);

    uart_print_data("CLOCK_ROOT_MEASURED",      Platform.CLOCK[0x0].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_EMMC_MEASURED",      Platform.CLOCK[0x1].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_UART_MEASURED",      Platform.CLOCK[0x2].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_ARM_MEASURED",       Platform.CLOCK[0x3].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_CORE_MEASURED",      Platform.CLOCK[0x4].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_V3D_MEASURED",       Platform.CLOCK[0x5].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_H264_MEASURED",      Platform.CLOCK[0x6].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_ISP_MEASURED",       Platform.CLOCK[0x7].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_SDRAM_MEASURED",     Platform.CLOCK[0x8].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_PIXEL_MEASURED",     Platform.CLOCK[0x9].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_PWM_MEASURED",       Platform.CLOCK[0xA].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_HEVC_MEASURED",      Platform.CLOCK[0xB].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_EMMC2_MEASURED",     Platform.CLOCK[0xC].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_M2MC_MEASURED",      Platform.CLOCK[0xD].MEASURED, "Hz (32-bit)");
    uart_print_data("CLOCK_PIXEL_BVB_MEASURED", Platform.CLOCK[0xE].MEASURED, "Hz (32-bit)");
    ignore();
}

void clock_read(VALUE id)
{
    ADDRESS i = 1;
//    ADDRESS a = 0;
//    ADDRESS b = 0;
    ADDRESS c = 0;
//    ADDRESS d = 0;
//    ADDRESS e = 0;

    Platform.Mailbox[i++] = 0;          //Mailbox Request
/*
    Platform.Mailbox[i++] = 0x00030001; //MBOX_TAG_GET_CLOCK_STATE
    Platform.Mailbox[i++] = 4;          //Request Length (bytes)
    Platform.Mailbox[i++] = 8;          //Response Length (bytes)
    Platform.Mailbox[i++] = id;         //Clock ID
a=i;Platform.Mailbox[i++] = 0;          //State (Bit 0: Off/On, Bit 1: Exists/Missing)

    Platform.Mailbox[i++] = 0x00030002; //MBOX_TAG_GET_CLOCK_RATE
    Platform.Mailbox[i++] = 4;          //Request Length (bytes)
    Platform.Mailbox[i++] = 8;          //Response Length (bytes)
    Platform.Mailbox[i++] = id;         //Clock ID
b=i;Platform.Mailbox[i++] = 0;          //Rate (Hz capped to 4.3GHz for 32-bit Mailbox)
*/
    Platform.Mailbox[i++] = 0x00030047; //MBOX_TAG_GET_CLOCK_MEASURED
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = id;         //Clock ID
c=i;Platform.Mailbox[i++] = 0;          //Rate (Hz capped to 4.3GHz for 32-bit Mailbox)
/*
    Platform.Mailbox[i++] = 0x00030004; //MBOX_TAG_GET_MAX_CLOCK_RATE
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = id;         //Clock ID
d=i;Platform.Mailbox[i++] = 0;          //Rate (Hz capped to 4.3GHz for 32-bit Mailbox)

    Platform.Mailbox[i++] = 0x00030007; //MBOX_TAG_GET_MIN_CLOCK_RATE
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = id;         //Clock ID
e=i;Platform.Mailbox[i++] = 0;          //Rate (Hz capped to 4.3GHz for 32-bit Mailbox)
*/
    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8))
    {
        post_code(0xB1);
        //Platform.CLOCK[id].STATE    = Platform.Mailbox[a];
        //post_code(0xB2);
        //Platform.CLOCK[id].RATE     = Platform.Mailbox[b];
        //post_code(0xB3);
        Platform.CLOCK[id].MEASURED = Platform.Mailbox[c];
        //post_code(0xB4);
        //Platform.CLOCK[id].MAX      = Platform.Mailbox[d];
        //post_code(0xB5);
        //Platform.CLOCK[id].MIN      = Platform.Mailbox[e];
        post_code(0xB9);
    }
}

void clock_write(VALUE id, VALUE state, VALUE rate) //!!! This might not work correctly
{
    ADDRESS i = 1;

    Platform.Mailbox[i++] = 0;          //Mailbox Request
/*
    Platform.Mailbox[i++] = 0x00038001; //MBOX_TAG_SET_CLOCK_STATE
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = id;         //Clock ID
    Platform.Mailbox[i++] = state;      //State (Bit 0: Off/On, Bit 1: Exists/Missing)
*/
    Platform.Mailbox[i++] = 0x00038002; //MBOX_TAG_SET_CLOCK_RATE
    Platform.Mailbox[i++] = 3 * 4;      //Request Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Response Length (bytes)
    Platform.Mailbox[i++] = id;         //Clock ID
    Platform.Mailbox[i++] = rate;       //Rate (Hz capped to 4.3GHz for 32-bit Mailbox)
    Platform.Mailbox[i++] = 0;          //Skip Setting Turbo? (Yes/No)

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS != mbox_setup(8))  //Call clock_read() manually to update the values
    {
        uart_write("[WARN]: !!! OVERCLOCK MAILBOX FAILURE !!!\n");
    }
}

void clock_read_turbo(void)
{
    ADDRESS i = 1;
    ADDRESS a = 0;

    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00038009; //MBOX_TAG_GET_TURBO
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 0;          //Clock ID
a=i;Platform.Mailbox[i++] = 0;          //Turbo Level

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8))
    {
        Platform.CLOCK_TURBO = Platform.Mailbox[a];
    }
}

void clock_write_turbo(VALUE level)
{
    ADDRESS i = 1;

    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00030009; //MBOX_TAG_SET_TURBO
    Platform.Mailbox[i++] = 8;          //Data Length (bytes)
    Platform.Mailbox[i++] = 2 * 4;      //Data Length
    Platform.Mailbox[i++] = 0;          //Clock ID
    Platform.Mailbox[i++] = level;      //Turbo Level

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    mbox_setup(8);
}

void clock_update(void)
{
    for (VALUE id = 0; id < CLOCK_LIMIT; ++id) { clock_read(id); }
    clock_read_turbo();
}