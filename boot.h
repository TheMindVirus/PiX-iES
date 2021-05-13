#ifndef _BOOT_H_
#define _BOOT_H_

#define	PERI_BASE		0x0000000020000000

#define	GPIO_PINS		53
#define	GPIO_BASE		0x0000000000200000 + PERI_BASE
#define GPIO_GPFSEL0		0x0000000000000000 + GPIO_BASE
#define GPIO_GPSET0		0x000000000000001C + GPIO_BASE
#define GPIO_GPCLR0		0x0000000000000028 + GPIO_BASE
#define GPIO_GPPUPPDN0		0x00000000000000E4 + GPIO_BASE

#define UART_BAUD		115200
#define UART_RATE		(500000000 / (UART_BAUD * 8)) - 1
#define	UART_BASE		0x0000000000215000 + PERI_BASE
#define	UART_AUX_ENABLES	0x0000000000000004 + UART_BASE
#define	UART_AUX_MU_IO_REG	0x0000000000000040 + UART_BASE
#define	UART_AUX_MU_IIR_REG	0x0000000000000048 + UART_BASE
#define	UART_AUX_MU_LCR_REG	0x000000000000004C + UART_BASE
#define	UART_AUX_MU_LSR_REG	0x0000000000000054 + UART_BASE
#define	UART_AUX_MU_CNTL_REG	0x0000000000000060 + UART_BASE
#define	UART_AUX_MU_BAUD_REG	0x0000000000000068 + UART_BASE

#define MBOX_BASE		0x000000000000B880 + PERI_BASE
#define MBOX_READ		0x0000000000000000 + MBOX_BASE
#define MBOX_STATUS		0x0000000000000018 + MBOX_BASE
#define MBOX_WRITE		0x0000000000000020 + MBOX_BASE

#endif//_BOOT_H_