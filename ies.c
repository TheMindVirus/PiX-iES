#include "ies.h"

PLATFORM Platform;

void platform_setup(void)
{
    Platform.STATE_GPIO = 0;
    Platform.PERI_BASE = 0x20000000; //Pi0&1
  //Platform.PERI_BASE = 0x3F000000; //Pi2&3
  //Platform.PERI_BASE = 0xFE000000; //Pi4

    Platform.GPIO_PINS = 53;
    Platform.GPIO_BASE = Platform.PERI_BASE + 0x00200000;
    Platform.GPFSEL0   = Platform.GPIO_BASE + 0x00000000;
    Platform.GPSET0    = Platform.GPIO_BASE + 0x0000001C;
    Platform.GPCLR0    = Platform.GPIO_BASE + 0x00000028;
    Platform.GPPUPPDN0 = Platform.GPIO_BASE + 0x000000E4;

    Platform.UART_CLOCK      = 500000000;
    Platform.UART_COUNTER    = 0;
    Platform.AUX_BASE        = Platform.PERI_BASE + 0x00215000;
    Platform.AUX_IRQ         = Platform.AUX_BASE  + 0x00000000;
    Platform.AUX_ENABLES     = Platform.AUX_BASE  + 0x00000004;
    Platform.AUX_MU_IO_REG   = Platform.AUX_BASE  + 0x00000040;
    Platform.AUX_MU_IER_REG  = Platform.AUX_BASE  + 0x00000044;
    Platform.AUX_MU_IIR_REG  = Platform.AUX_BASE  + 0x00000048;
    Platform.AUX_MU_LCR_REG  = Platform.AUX_BASE  + 0x0000004C;
    Platform.AUX_MU_MCR_REG  = Platform.AUX_BASE  + 0x00000050;
    Platform.AUX_MU_LSR_REG  = Platform.AUX_BASE  + 0x00000054;
    Platform.AUX_MU_CNTL_REG = Platform.AUX_BASE  + 0x00000060;
    Platform.AUX_MU_BAUD_REG = Platform.AUX_BASE  + 0x00000068;

    Platform.MBOX_BASE   = Platform.PERI_BASE + 0x0000B880;
    Platform.MBOX_READ   = Platform.MBOX_BASE + 0x00000000;
    Platform.MBOX_STATUS = Platform.MBOX_BASE + 0x00000018;
    Platform.MBOX_WRITE  = Platform.MBOX_BASE + 0x00000020;

    Platform.MONITOR_FRAMEBUFFER = 0xFE2FE000; //Temporary, will be rewritten...
    Platform.MONITOR_PITCH_SPACE = 0x00000000;

    Platform.MONITOR_NUM_DISPLAYS = 0;
    for (ADDRESS i = 0; i < 5; ++i) { Platform.MONITOR_DISPLAY_ID[i] = 0; }
    
    Platform.MONITOR_FRAMEBUFFER2 = 0xFE2FE000; //Temporary, will be rewritten...
    Platform.MONITOR_PITCH_SPACE2 = 0x00000000;

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

VALUE mbox_get_vsync(void)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    VALUE retval = 0;

    Platform.Mailbox[i++] = 0;            //Mailbox Request

    Platform.Mailbox[i++] = 0x0004400e;   //MBOX_TAG_TEST_VSYNC
    Platform.Mailbox[i++] = 4;            //Data Length (request?)
    Platform.Mailbox[i++] = 4;            //Data Length (response?)
a=i;Platform.Mailbox[i++] = 0;            //Value

    Platform.Mailbox[i++] = 0;            //End Mark
    Platform.Mailbox[0] = i * 4;          //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8)) { retval = Platform.Mailbox[a]; }
    else { uart_write("\n[WARN]: Mailbox Transaction Error\n"); }
    return retval;
}

void mbox_set_vsync(VALUE enabled)
{
    ADDRESS i = 1;

    Platform.Mailbox[i++] = 0;            //Mailbox Request

    Platform.Mailbox[i++] = 0x0004800e;   //MBOX_TAG_SET_VSYNC
    Platform.Mailbox[i++] = 4;            //Data Length (bytes)
    Platform.Mailbox[i++] = 4;            //Data Length (again)
    Platform.Mailbox[i++] = enabled;      //Value

    Platform.Mailbox[i++] = 0;            //End Mark
    Platform.Mailbox[0] = i * 4;          //Update Packet Size

    if (MBOX_SUCCESS != mbox_setup(8)) { uart_write("\n[WARN]: Mailbox Transaction Error\n"); }
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

void mbox_get_edid(void)
{
    ADDRESS i = 1;
    ADDRESS a = 0;
    ADDRESS b = 0;
    ADDRESS c = 0;
    VALUE finished = 0;
    VALUE block = 0;
    ELEMENT X = 0;
    ADDRESS status = 0;
    ADDRESS size = 0;
    ADDRESS offset = 0;
    
    while (finished == 0)
    {
zero_packet(MAX_SIZE);
        Platform.Mailbox[i++] = 0;            //Mailbox Request

        Platform.Mailbox[i++] = 0x00030020;   //MBOX_TAG_GET_EDID_BLOCK
        Platform.Mailbox[i++] = 4;            //Data Length (request?)
    a=i;Platform.Mailbox[i++] = 136;          //Data Length (response?)
        Platform.Mailbox[i++] = block;        //Block Number
    b=i;Platform.Mailbox[i++] = 0;            //Status
    c=i;Platform.Mailbox[i++] = 0;            //Data

        for (ADDRESS j = 1; j < 136; ++j) { Platform.Mailbox[i++] = 0; } //More Data

        Platform.Mailbox[i++] = 0;            //End Mark
        Platform.Mailbox[0] = i * 4;          //Update Packet Size

        if (MBOX_SUCCESS == mbox_setup(8))
        {
            block = block + 1;//++block;
            X = X + 1;
            status = Platform.Mailbox[b];
            if ((block > (MAX_SIZE / 136))
            ||  (status != 0)) { finished = 1; }
            
            uart_print(block);
            uart_write("   ");
            uart_print(MAX_SIZE / 136);
            uart_write("   ");
            uart_write((((VALUE)block > (VALUE)(MAX_SIZE / 136)) ? "true" : "false"));
            uart_write("   ");
            uart_write(((block > 0xB) ? "true" : "false"));
            uart_write("   ");
            uart_write(((X > 0xB) ? "true" : "false"));
            uart_write("\n");

            size = Platform.Mailbox[a] & 0x3FFFFFFF;
            offset += size / 4;
            for (ADDRESS j = 0; j < size / 4; ++j) { Platform.EDID[offset + j] = Platform.Mailbox[c + j]; }
        }
        else
        {
            uart_write("\n[WARN]: Mailbox Transaction Error\n");
            Platform.EDID[0] = 0;
            Platform.EDIDsize = 0;
            return;
        }
        //print_packet(size / 4);
    }
    Platform.EDIDsize = offset;
}