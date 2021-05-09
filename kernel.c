#include <stddef.h>
#include <stdint.h>
 
static uint32_t MMIO_BASE;

// The MMIO area base address, depends on board type
static inline void mmio_init(int raspi)
{
    switch (raspi) {
        case 2:
        case 3:  MMIO_BASE = 0x3F000000; break; // for raspi2 & 3
        case 4:  MMIO_BASE = 0xFE000000; break; // for raspi4
        default: MMIO_BASE = 0x20000000; break; // for raspi1, raspi zero etc.
    }
}

// Memory-Mapped I/O output
static inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t*)(MMIO_BASE + reg) = data;
}
 
// Memory-Mapped I/O input
static inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t*)(MMIO_BASE + reg);
}

// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int32_t count)
{
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : "=r"(count): [count]"0"(count) : "cc");
}
 
enum
{
    // The offsets for reach register.
    GPIO_BASE = 0x200000,
 
    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),
 
    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),
 
    // The base address for UART.
    UART0_BASE = (GPIO_BASE + 0x1000), // for raspi4 0xFE201000, raspi2 & 3 0x3F201000, and 0x20201000 for raspi1
 
    // The offsets for reach register for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
 
    // The offsets for Mailbox registers
    MBOX_BASE    = 0xB880,
    MBOX_READ    = (MBOX_BASE + 0x00),
    MBOX_STATUS  = (MBOX_BASE + 0x18),
    MBOX_WRITE   = (MBOX_BASE + 0x20)
};
 
// A Mailbox message with set clock rate of PL011 to 3MHz tag
volatile unsigned int  __attribute__((aligned(16))) mbox[9] = {
    9*4, 0, 0x38002, 12, 8, 2, 3000000, 0 ,0
};
 
void uart_init(int raspi)
{
	mmio_init(raspi);
 
	// Disable UART0.
	mmio_write(UART0_CR, 0x00000000);
	// Setup the GPIO pin 14 && 15.
 
	// Disable pull up/down for all GPIO pins & delay for 150 cycles.
	mmio_write(GPPUD, 0x00000000);
	delay(150);
 
	// Disable pull up/down for pin 14,15 & delay for 150 cycles.
	mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
 
	// Write 0 to GPPUDCLK0 to make it take effect.
	mmio_write(GPPUDCLK0, 0x00000000);
 
	// Clear pending interrupts.
	mmio_write(UART0_ICR, 0x7FF);
 
	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// Baud = 115200.
 
	// For Raspi3 and 4 the UART_CLOCK is system-clock dependent by default.
	// Set it to 3Mhz so that we can consistently set the baud rate
	if (raspi >= 3) {
		// UART_CLOCK = 30000000;
		unsigned int r = (((unsigned int)(&mbox) & ~0xF) | 8);
		// wait until we can talk to the VC
		while ( mmio_read(MBOX_STATUS) & 0x80000000 ) { }
		// send our message to property channel and wait for the response
		mmio_write(MBOX_WRITE, r);
		while ( (mmio_read(MBOX_STATUS) & 0x40000000) || mmio_read(MBOX_READ) != r ) { }
	}
 
	// Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
	mmio_write(UART0_IBRD, 1);
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	mmio_write(UART0_FBRD, 40);
 
	// Enable FIFO & 8 bit data transmission (1 stop bit, no parity).
	mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}
 
void uart_putc(unsigned char c)
{
	// Wait for UART to become ready to transmit.
	while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	mmio_write(UART0_DR, c);
}
 
unsigned char uart_getc()
{
    // Wait for UART to have received something.
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}
 
void uart_puts(const char* str)
{
	for (size_t i = 0; str[i] != '\0'; i ++)
		uart_putc((unsigned char)str[i]);
}

void uart_puth(unsigned int value)
{
    uart_puts("0x");
    unsigned char map[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    for (unsigned int i = sizeof(unsigned int) * 8; i > 0; i -= 4)
    {
        uart_putc(map[((value >> (i - 4)) & 0xF)]);
    }
}

static unsigned int fb_width = 0;
static unsigned int fb_height = 0;
static unsigned int fb_address = 0;
static unsigned int fb_pitch = 0;

#define drawPixel(x, y, colour) \
    mmio_write((fb_address + (y * fb_pitch) + (x * 4)), colour);

void mbox_get_framebuffer(void);
unsigned int mbox_setup(unsigned char channel);
void mbox_print_packet(unsigned int size);

unsigned int __attribute__((aligned(4))) mailbox[1500] __attribute__((aligned(16)));

#define MBOX_EMPTY     0x40000000
#define MBOX_FULL      0x80000000
#define MBOX_FAILURE   0x80000001
#define MBOX_SUCCESS   0x80000000

#define mbox_peek()           (mmio_read(MBOX_STATUS))
#define mbox_read()           (mmio_read(MBOX_READ))
#define mbox_write(addrech)   (mmio_write(MBOX_WRITE, addrech))

void fb_init(void)
{
    mbox_get_framebuffer();
    mbox_print_packet(mailbox[0]);

    uart_puts("[INFO]: Width (Hex): ");
    uart_puth(fb_width);
    uart_puts("\n");

    uart_puts("[INFO]: Height (Hex): ");
    uart_puth(fb_height);
    uart_puts("\n");

    uart_puts("[INFO]: Framebuffer: ");
    uart_puth(fb_address);
    uart_puts("\n");

    //QEMU Not Implemented Bug
    if (fb_pitch == 0) { fb_pitch = fb_width * 4; }

    uart_puts("[INFO]: Pitch Space: ");
    uart_puth(fb_pitch);
    uart_puts("\n\n");

    for (unsigned int y = 0; y < fb_height; ++y)
    {
        for (unsigned int x = 0; x < fb_width; ++x)
        {
            drawPixel(x, y, 0x0000AA);
        }
    }
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
 
#ifdef AARCH64
// arguments for AArch64
void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
#else
// arguments for AArch32
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
#endif
{
	// initialize UART for Raspi2
	uart_init(2);
	uart_puts("[INFO]: Hello, kernel World!\r\n");
	fb_init();
 
	while (1) { uart_putc(uart_getc()); }
        r0 = r0;
        r1 = r1;
        atags = atags;
}

unsigned int mbox_setup(unsigned char channel)
{
    unsigned int checked = 0;
    unsigned int mail = ((((unsigned int)mailbox) &~ 0xF) | (channel & 0xF)); //0xF reserved for 4-bit channel
   
    int t = 0;
    while ((mbox_peek() & MBOX_FULL) != 0) { ++t; if (t > 100) { uart_puts("[WARN]: Mailbox Error 1\n"); break; } }
    mbox_write(mail);
    while (1)
    {
        while((mbox_peek() & MBOX_EMPTY) != 0) { ++t; if (t > 100) { uart_puts("[WARN]: Mailbox Error 2\n"); break; } }
        checked = mbox_read();
        if (mail == checked) { return MBOX_SUCCESS; }
        ++t; if (t > 100) { uart_puts("[WARN]: Mailbox Error 3\n"); break; }
    }
    uart_puts("[WARN]: Mailbox Error 4\n");
    return MBOX_FAILURE;
}

void mbox_print_packet(unsigned int size)
{
    size = size / sizeof(unsigned int);
    for (unsigned int i = 0; i < size; ++i)
    {
        uart_puts("[MBOX]: mailbox[");
        uart_puth(i * sizeof(unsigned int));
        uart_puts("] = ");
        uart_puth(mailbox[i]);
        uart_puts("\n");
    }
    uart_puts("\n");
}

void mbox_get_framebuffer(void)
{
    unsigned int i = 1;
    unsigned int a = 0;
    unsigned int b = 0;
    unsigned int w = 0;
    unsigned int h = 0;

    mailbox[i++] = 0;          //Mailbox Request

    mailbox[i++] = 0x00048003; //MBOX_TAG_SETPHYHW
    mailbox[i++] = 8;          //Buffer Length
    mailbox[i++] = 8;          //Data Length
    mailbox[i++] = 1024;       //Value
    mailbox[i++] = 768;        //Value

    mailbox[i++] = 0x00048004; //MBOX_TAG_SETVIRTHW
    mailbox[i++] = 8;          //Buffer Length
    mailbox[i++] = 8;          //Data Length
w=i;mailbox[i++] = 1024;       //Value
h=i;mailbox[i++] = 768;        //Value

    mailbox[i++] = 0x00048009; //MBOX_TAG_SETVIRTOFF
    mailbox[i++] = 8;          //Buffer Length
    mailbox[i++] = 8;          //Data Length
    mailbox[i++] = 0;          //Value
    mailbox[i++] = 0;          //Value

    mailbox[i++] = 0x00048005; //MBOX_TAG_SETDEPTH
    mailbox[i++] = 8;          //Buffer Length
    mailbox[i++] = 8;          //Data Length
    mailbox[i++] = 0;          //Value
    mailbox[i++] = 0;          //Value

    mailbox[i++] = 0x00048006; //MBOX_TAG_SETXPLODR
    mailbox[i++] = 4;          //Buffer Length
    mailbox[i++] = 4;          //Data Length
    mailbox[i++] = 1;          //Value

    mailbox[i++] = 0x00040001; //MBOX_TAG_GETFB
    mailbox[i++] = 8;          //Buffer Length
    mailbox[i++] = 8;          //Data Length
a=i;mailbox[i++] = 4096;       //Value
    mailbox[i++] = 0;          //Value

    mailbox[i++] = 0x00040008; //MBOX_TAG_GETPITCH
    mailbox[i++] = 4;          //Buffer Length
    mailbox[i++] = 4;          //Data Length
b=i;mailbox[i++] = 0;          //Value

    mailbox[i++] = 0;          //End Mark
    mailbox[0  ] = i * 4;      //Update Packet Size

    if (MBOX_SUCCESS == mbox_setup(8))
    {
        fb_width = mailbox[w];
        fb_height = mailbox[h];
        fb_address = mailbox[a] & 0x3FFFFFFF; //Translate from VC to ARM address
        fb_pitch = mailbox[b];
    }
}