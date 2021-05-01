#include "iES.H"

PLATFORM Platform;

void platform_setup(void)
{
    Platform.STATE_GPIO = 0;
    Platform.PERI_BASE = 0x00000000FE000000;

    Platform.GPIO_PINS = 53;
    Platform.GPIO_BASE = Platform.PERI_BASE + 0x0000000000200000;
    Platform.GPFSEL0   = Platform.GPIO_BASE + 0x0000000000000000;
    Platform.GPSET0    = Platform.GPIO_BASE + 0x000000000000001C;
    Platform.GPCLR0    = Platform.GPIO_BASE + 0x0000000000000028;
    Platform.GPPUPPDN0 = Platform.GPIO_BASE + 0x00000000000000E4;

    Platform.UART_CLOCK      = 500000000;
    Platform.UART_COUNTER    = 0;
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

    for (ADDRESS i = 0; i < MAX_SIZE; ++i)
    {
        Platform.Mailbox[i] = 0;
        Platform.EDID[i] = 0;
    }

    Platform.EDIDsize = 0;
}

void gpio_setup(ADDRESS pin, VALUE value, ADDRESS base, VALUE size)
{
    Platform.STATE_GPIO = 0;
    VALUE mask = (1 << size) - 1;
    if (pin > Platform.GPIO_PINS) { Platform.STATE_GPIO = 2; return; }
    if (value > mask) { Platform.STATE_GPIO = 3; return; }

    VALUE fields = 32 / size;
    ADDRESS address = base + ((pin / fields) * 4);
    VALUE shift = (pin % fields) * size;

    VALUE edit = mmio_read(address);
    edit &= ~(mask << shift);
    edit |= value << shift;
    mmio_write(address, edit);
    Platform.STATE_GPIO = 1;
}

void uart_setup(VALUE baud)
{
    gpio_pull(14, GPIO_PULL_NONE);
    gpio_alt(14, GPIO_ALT5);
    gpio_pull(15, GPIO_PULL_NONE);
    gpio_alt(15, GPIO_ALT5);

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

void uart_print(VALUE value)
{
    uart_write("0x");
    BYTE map[] = "0123456789ABCDEF";
    for (ADDRESS i = sizeof(VALUE) * 8; i > 0; i -= 4)
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
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 1080;       //Value

    Platform.Mailbox[i++] = 0x00048004; //MBOX_TAG_SETVIRTHW
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 1080;       //Value

    Platform.Mailbox[i++] = 0x00048009; //MBOX_TAG_SETVIRTOFF
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
    Platform.Mailbox[i++] = 0;          //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048005; //MBOX_TAG_SETDEPTH
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
    Platform.Mailbox[i++] = 0;          //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048006; //MBOX_TAG_SETXPLODR
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
    Platform.Mailbox[i++] = 1;          //Value

    Platform.Mailbox[i++] = 0x00040001; //MBOX_TAG_GETFB
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
a=i;Platform.Mailbox[i++] = 4096;       //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00040008; //MBOX_TAG_GETPITCH
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
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
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
a=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00040016; //MBOX_TAG_GET_DISPLAY_ID
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
b=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048013; //MBOX_TAG_SET_DISPLAY_NUM
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
    Platform.Mailbox[i++] = 1;          //Value //Change to 1 if it doesn't work

    Platform.Mailbox[i++] = 0x00040016; //MBOX_TAG_GET_DISPLAY_ID
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
c=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8))
    {
        Platform.MONITOR_NUM_DISPLAYS  = Platform.Mailbox[a];
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
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 1080;       //Value

    Platform.Mailbox[i++] = 0x00048004; //MBOX_TAG_SETVIRTHW
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 1080;       //Value

    Platform.Mailbox[i++] = 0x00048009; //MBOX_TAG_SETVIRTOFF
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
    Platform.Mailbox[i++] = 1920;       //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048005; //MBOX_TAG_SETDEPTH
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
    Platform.Mailbox[i++] = 0;          //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00048006; //MBOX_TAG_SETXPLODR
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
    Platform.Mailbox[i++] = 1;          //Value

    Platform.Mailbox[i++] = 0x00040001; //MBOX_TAG_GETFB
    Platform.Mailbox[i++] = 8;          //Buffer Length
    Platform.Mailbox[i++] = 8;          //Data Length
a=i;Platform.Mailbox[i++] = 4096;       //Value
    Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0x00040008; //MBOX_TAG_GETPITCH
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
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

void print_packet(VALUE size)
{
    for (ADDRESS i = 0; i < size; ++i)
    {
        uart_write("[MBOX]: Platform.Mailbox[");
        uart_print((VALUE)i);
        uart_write("] = ");
        uart_print((VALUE)Platform.Mailbox[i]);
        uart_write("\n");
    }
    uart_write("\n");
}

void zero_packet(VALUE size) { for (ADDRESS i = 0; i < size; ++i) { Platform.Mailbox[i] = 0; } }

VALUE mem_alloc(VALUE size, VALUE align, VALUE flags)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x0003000C; //MBOX_TAG_ALLOCATE_MEMORY
    Platform.Mailbox[i++] = 12;         //Buffer Length
    Platform.Mailbox[i++] = 12;         //Data Length
a=i;Platform.Mailbox[i++] = size;       //Value
    Platform.Mailbox[i++] = align;      //Value
    Platform.Mailbox[i++] = flags;      //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { return Platform.Mailbox[a]; }
    return 0;
}

VALUE mem_free(VALUE handle)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x0003000F; //MBOX_TAG_RELEASE_MEMORY
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
a=i;Platform.Mailbox[i++] = handle;     //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { return Platform.Mailbox[a]; }
    return 0;
}

VALUE mem_lock(VALUE handle)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x0003000D; //MBOX_TAG_LOCK_MEMORY
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
a=i;Platform.Mailbox[i++] = handle;     //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { return Platform.Mailbox[a]; }
    return 0;
}

VALUE mem_unlock(VALUE handle)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x0003000E; //MBOX_TAG_UNLOCK_MEMORY
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
a=i;Platform.Mailbox[i++] = handle;     //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { return Platform.Mailbox[a]; }
    return 0;
}

VALUE qpu_setup(void)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00030012; //MBOX_TAG_SET_ENABLE_QPU
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
a=i;Platform.Mailbox[i++] = 1;          //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { return Platform.Mailbox[a]; }
    return 0;
}

VALUE qpu_release(void)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00030012; //MBOX_TAG_SET_ENABLE_QPU
    Platform.Mailbox[i++] = 4;          //Buffer Length
    Platform.Mailbox[i++] = 4;          //Data Length
a=i;Platform.Mailbox[i++] = 0;          //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { return Platform.Mailbox[a]; }
    return 0;
}

VALUE qpu_code(VALUE code, VALUE r0, VALUE r1, VALUE r2, VALUE r3, VALUE r4, VALUE r5)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00030010; //MBOX_TAG_EXECUTE_CODE
    Platform.Mailbox[i++] = 28;         //Buffer Length
    Platform.Mailbox[i++] = 28;         //Data Length
a=i;Platform.Mailbox[i++] = code;       //Value
    Platform.Mailbox[i++] = r0;         //Value
    Platform.Mailbox[i++] = r1;         //Value
    Platform.Mailbox[i++] = r2;         //Value
    Platform.Mailbox[i++] = r3;         //Value
    Platform.Mailbox[i++] = r4;         //Value
    Platform.Mailbox[i++] = r5;         //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { return Platform.Mailbox[a]; }
    return 0;
}

VALUE qpu_run(VALUE num_qpus, VALUE control, VALUE no_flush, VALUE timeout)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    Platform.Mailbox[i++] = 0;          //Mailbox Request

    Platform.Mailbox[i++] = 0x00030011; //MBOX_TAG_EXECUTE_QPU
    Platform.Mailbox[i++] = 16;         //Buffer Length
    Platform.Mailbox[i++] = 16;         //Data Length
a=i;Platform.Mailbox[i++] = num_qpus;   //Value
    Platform.Mailbox[i++] = control;    //Value
    Platform.Mailbox[i++] = no_flush;   //Value
    Platform.Mailbox[i++] = timeout;    //Value

    Platform.Mailbox[i++] = 0;          //End Mark
    Platform.Mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { return Platform.Mailbox[a]; }
    return 0;
}

void qpu_test(void)
{
    uart_write("[TEST]: QPU");
    qpu_setup();
    //qpu_code();
    //qpu_run();
    qpu_release();
}