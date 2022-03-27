#include "iES.H"

PLATFORM Platform;

void platform_setup(void)
{
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
    BYTE map[] = "0123456789ABCDEF";
    for (ADDRESS i = sizeof(ADDRESS) * 8; i > 0; i -= 4)
    {
        uart_write_byte(map[((value >> (i - 4)) & 0xF)]);
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