#ifndef IES_H
#define IES_H

#define STATIC   static
#define REFER         *
#define PACK(ALIGNMENT)   __attribute__((aligned(ALIGNMENT)))
#define MAX_SIZE   1500

typedef unsigned long long   ADDRESS;
typedef unsigned int         VALUE;
typedef volatile VALUE       ELEMENT;
typedef ELEMENT REFER        POINTER;
typedef unsigned char        BYTE;
typedef BYTE REFER           QUEUE;
typedef BYTE REFER           BUFFER;
typedef const char REFER     LITERAL;
typedef VALUE PACK(4)        MAILBOX;

typedef struct _PLATFORM
{
    VALUE     STATE_GPIO;
    ADDRESS   PERI_BASE;

    VALUE     GPIO_PINS;
    ADDRESS   GPIO_BASE;
    ADDRESS   GPFSEL0;
    ADDRESS   GPSET0;
    ADDRESS   GPCLR0;
    ADDRESS   GPPUPPDN0;

    VALUE     UART_CLOCK;
    ADDRESS   UART_COUNTER;
    BYTE      UART_QUEUE[MAX_SIZE];
    ADDRESS   AUX_BASE;
    ADDRESS   AUX_IRQ;
    ADDRESS   AUX_ENABLES;
    ADDRESS   AUX_MU_IO_REG;
    ADDRESS   AUX_MU_IER_REG;
    ADDRESS   AUX_MU_IIR_REG;
    ADDRESS   AUX_MU_LCR_REG;
    ADDRESS   AUX_MU_MCR_REG;
    ADDRESS   AUX_MU_LSR_REG;
    ADDRESS   AUX_MU_CNTL_REG;
    ADDRESS   AUX_MU_BAUD_REG;

    MAILBOX   Mailbox[MAX_SIZE]   PACK(16); //0xF reserved for 4-bit channel
    ADDRESS   MBOX_BASE;
    ADDRESS   MBOX_READ;
    ADDRESS   MBOX_STATUS;
    ADDRESS   MBOX_WRITE;

    ADDRESS   MONITOR_FRAMEBUFFER;
    VALUE     MONITOR_PITCH_SPACE;
    
}   PLATFORM, REFER PPLATFORM;
extern PLATFORM Platform;

enum GPIO_ALT
{
    GPIO_INPUT  = 0b000,
    GPIO_OUTPUT = 0b001,
    GPIO_ALT0   = 0b100,
    GPIO_ALT1   = 0b101,
    GPIO_ALT2   = 0b110,
    GPIO_ALT3   = 0b111,
    GPIO_ALT4   = 0b011,
    GPIO_ALT5   = 0b010,
};

enum GPIO_PULL
{
    GPIO_PULL_NONE = 0b00,
    GPIO_PULL_UP   = 0b01,
    GPIO_PULL_DOWN = 0b10,
    GPIO_PULL_BOTH = 0b11,
};

enum MBOX_FLAGS
{
    MBOX_REQUEST = 0x00000000,
    MBOX_EMPTY   = 0x40000000,
    MBOX_FULL    = 0x80000000,
    MBOX_SUCCESS = 0x80000000,
    MBOX_FAILURE = 0x80000001,
};

void platform_setup(void);
#define mmio_read(address)   (REFER(POINTER)address)
#define mmio_write(address, value)   (REFER(POINTER)address = value)

void gpio_setup(ADDRESS pin, VALUE value, ADDRESS base, VALUE size);
#define gpio_set(pin) (gpio_setup(pin, 1, Platform.GPSET0, 1))
#define gpio_clear(pin) (gpio_setup(pin, 1, Platform.GPCLR0, 1))
#define gpio_pull(pin, pull) (gpio_setup(pin, pull, Platform.GPPUPPDN0, 2))
#define gpio_alt(pin, alt) (gpio_setup(pin, alt, Platform.GPFSEL0, 3))

void uart_setup(VALUE baud);
#define uart_idle()   ((mmio_read(Platform.AUX_MU_LSR_REG) & 0x20) > 0)
#define uart_peek()   ((mmio_read(Platform.AUX_MU_LSR_REG) & 0x01) > 0)
#define uart_read_byte()   (mmio_read(Platform.AUX_MU_IO_REG) & 0xFF)
LITERAL uart_read(ADDRESS length);
void uart_write_byte(BYTE byte);
void uart_write_buffer(BUFFER buffer);
#define uart_write(message)   (uart_write_buffer((BUFFER)message))
void uart_loop(void);
void uart_print(VALUE value);

VALUE mbox_setup(BYTE channel);
#define mbox_peek()   (mmio_read(Platform.MBOX_STATUS))
#define mbox_read()   (mmio_read(Platform.MBOX_READ))
#define mbox_write(addrech)   (mmio_write(Platform.MBOX_WRITE, addrech))
void mbox_get_framebuffer(void);

#endif//IES_H