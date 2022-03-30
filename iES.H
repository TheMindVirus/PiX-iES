#ifndef IES_H
#define IES_H

#define CLOCK_LIMIT   0xF

#define STATIC   static
#define GLOBAL   extern
#define REFER         *
#define PACK(ALIGNMENT)   __attribute__((aligned(ALIGNMENT)))
#define MAX_SIZE   1500
#define STRUCTURE  typedef struct

typedef unsigned long long   ADDRESS;
typedef unsigned int         VALUE;
typedef volatile VALUE       ELEMENT;
typedef ELEMENT REFER        POINTER;
typedef unsigned char        BYTE;
typedef BYTE REFER           QUEUE;
typedef BYTE REFER           BUFFER;
typedef const char REFER     LITERAL;
typedef VALUE PACK(4)        MAILBOX;
typedef unsigned char        GO8BIT;
typedef bool                 BOOLEAN;
typedef unsigned long long   TIME;

STRUCTURE _PLATFORM
{
    LITERAL   HEXMAP;

    ADDRESS   PERI_BASE;

    ADDRESS   GPIO_BASE;
    ADDRESS   GPIO_GPFSEL0;
    ADDRESS   GPIO_GPSET0;
    ADDRESS   GPIO_GPCLR0;
    ADDRESS   GPIO_GPLEV0;
    ADDRESS   GPIO_PUP_PDN_CNTRL_REG0;

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

    GO8BIT    UART_TX;
    GO8BIT    UART_RX;
    VALUE     UART_CLOCK;
    ADDRESS   UART_COUNTER;
    BYTE      UART_QUEUE[MAX_SIZE];

    MAILBOX   Mailbox[MAX_SIZE]   PACK(16); //0xF reserved for 4-bit channel
    ADDRESS   MBOX_BASE;
    ADDRESS   MBOX_READ;
    ADDRESS   MBOX_STATUS;
    ADDRESS   MBOX_WRITE;

    ADDRESS   MONITOR_FRAMEBUFFER;
    VALUE     MONITOR_PITCH_SPACE;

    VALUE     MONITOR_NUM_DISPLAYS;
    VALUE     MONITOR_DISPLAY_ID[5];
	
    ADDRESS   MONITOR_FRAMEBUFFER2;
    VALUE     MONITOR_PITCH_SPACE2;

    GO8BIT HEADPHONES_LEFT;
    GO8BIT HEADPHONES_RIGHT;

    GO8BIT DBUG_CARD_X1;
    GO8BIT DBUG_CARD_A1;
    GO8BIT DBUG_CARD_B1;
    GO8BIT DBUG_CARD_C1;
    GO8BIT DBUG_CARD_D1;
    GO8BIT DBUG_CARD_E1;
    GO8BIT DBUG_CARD_F1;
    GO8BIT DBUG_CARD_G1;

    GO8BIT DBUG_CARD_X2;
    GO8BIT DBUG_CARD_A2;
    GO8BIT DBUG_CARD_B2;
    GO8BIT DBUG_CARD_C2;
    GO8BIT DBUG_CARD_D2;
    GO8BIT DBUG_CARD_E2;
    GO8BIT DBUG_CARD_F2;
    GO8BIT DBUG_CARD_G2;

    ADDRESS TIMER_BASE;
    ADDRESS TIMER_COUNTER_LOW;
    ADDRESS TIMER_COUNTER_HIGH;
    TIME TIMER_CHECKPOINT_LAST;

    STRUCTURE _CLOCKDATA
    {
        VALUE ID;
        VALUE STATE;
        VALUE RATE;
        VALUE MEASURED;
        VALUE MAX;
        VALUE MIN;
    }   CLOCKDATA, REFER PCLOCKDATA;
    CLOCKDATA CLOCK[CLOCK_LIMIT];
    VALUE CLOCK_TURBO;
    
}   PLATFORM, REFER PPLATFORM;
GLOBAL PLATFORM Platform;

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

enum TIME_PERIODS //WARNING: SYSTEM TIMER IS PRONE TO 64-BIT OVERFLOW AND PRECISION ERROR!
{
    TIME_ONE_SECOND = 0x100000, // 1048576 ticks
    TIME_ONE_TENTH  = 0x1999A, // 104858 ticks
};

enum CLOCK_ID
{
    CLOCK_ID_ROOT      = 0x0,
    CLOCK_ID_EMMC      = 0x1,
    CLOCK_ID_UART      = 0x2,
    CLOCK_ID_ARM       = 0x3,
    CLOCK_ID_CORE      = 0x4,
    CLOCK_ID_V3D       = 0x5,
    CLOCK_ID_H264      = 0x6,
    CLOCK_ID_ISP       = 0x7,
    CLOCK_ID_SDRAM     = 0x8,
    CLOCK_ID_PIXEL     = 0x9,
    CLOCK_ID_PWM       = 0xA,
    CLOCK_ID_HEVC      = 0xB,
    CLOCK_ID_EMMC2     = 0xC,
    CLOCK_ID_M2MC      = 0xD,
    CLOCK_ID_PIXEL_BVB = 0xE,
};

enum CLOCK_STATE
{
    CLOCK_STATE_OFF_MISSING = 0b00,
    CLOCK_STATE_ON_MISSING  = 0b01, ///???
    CLOCK_STATE_OFF_PRESENT = 0b10,
    CLOCK_STATE_ON_PRESENT  = 0b11,
};

void platform_setup(void);
#define mmio_read(address)   (REFER(POINTER)(address))
#define mmio_write(address, value)   (REFER(POINTER)(address) = value)

void gpio_setup(ADDRESS pin, ADDRESS alt, ADDRESS pull);
#define gpio_read(pin) (((mmio_read(Platform.GPIO_GPLEV0 + (0x4 * (pin / 32))) >> (pin % 32)) & 1) == 1)
#define gpio_write(pin) (mmio_write(Platform.GPIO_GPSET0 + (0x4 * (pin / 32)), (1 << (pin % 32))))
#define gpio_reset(pin) (mmio_write(Platform.GPIO_GPCLR0 + (0x4 * (pin / 32)), (1 << (pin % 32))))

void uart_setup(VALUE baud);
#define uart_idle()   ((mmio_read(Platform.AUX_MU_LSR_REG) & 0x40) > 0)
#define uart_peek()   ((mmio_read(Platform.AUX_MU_LSR_REG) & 0x01) > 0)
#define uart_read_byte()   (mmio_read(Platform.AUX_MU_IO_REG) & 0xFF)
LITERAL uart_read(ADDRESS length);
void uart_write_byte(BYTE byte);
void uart_write_buffer(BUFFER buffer);
#define uart_write(message)   (uart_write_buffer((BUFFER)message))
void uart_loop(void);
void uart_print(ADDRESS value);
void uart_eval(VALUE value);
#define uart_print_data(LABEL, DATA, UNITS) \
    uart_write("["); uart_write(LABEL); uart_write("]:\t\t"); uart_eval(DATA); uart_write(UNITS); uart_write("\n");
#define uart_print_hex(LABEL, HEX, ITERATION) \
    uart_write("["); uart_write(LABEL); uart_write("]:\t\t"); uart_print(HEX); uart_write(" ("); uart_eval(ITERATION); uart_write(")\n");

VALUE mbox_setup(BYTE channel);
#define mbox_peek()   (mmio_read(Platform.MBOX_STATUS))
#define mbox_read()   (mmio_read(Platform.MBOX_READ))
#define mbox_write(addrech)   (mmio_write(Platform.MBOX_WRITE, addrech))
void mbox_get_framebuffer(void);
void mbox_get_mon_info(void);
void mbox_get_framebuffer2(void);

void print_memory(void);
void print_packet(VALUE n);
void zero_packet(VALUE n);

GO8BIT QCODE2DIGIT(GO8BIT Q, BOOLEAN DOT);
void post_code(GO8BIT qcode);
#define checkpoint(N) \
    post_code(0xC0 + N); uart_write("[CKPT]: "); uart_print(0xC0 + N); uart_write("\n");
#define ignore() checkpoint(9) //[C9]

TIME time_read(void);
void time_write(TIME checkpoint);
BOOLEAN time_check(TIME offset);
void time_wait(TIME ticks);

void clock_setup(void);
void clock_read(VALUE id);
void clock_write(VALUE id, VALUE state, VALUE rate);
void clock_read_turbo(void);
void clock_write_turbo(VALUE level);
void clock_update(void);

#endif//IES_H