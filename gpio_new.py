def main():
    gpio_setup(40, GPIO_OUTPUT, GPIO_PULL_NONE) #GPIO_PULL_BOTH
    gpio_setup(41, GPIO_OUTPUT, GPIO_PULL_NONE) #GPIO_PULL_BOTH
    gpio_write(40)
    print("[GPIO]: State: " + ("ON" if gpio_read(40) else "OFF"))
    gpio_reset(40)
    print("[GPIO]: State: " + ("ON" if gpio_read(40) else "OFF"))
    gpio_write(41)
    print("[GPIO]: State: " + ("ON" if gpio_read(41) else "OFF"))
    gpio_reset(41)
    print("[GPIO]: State: " + ("ON" if gpio_read(41) else "OFF"))
    gpio_write(40)
    gpio_write(41)
    mmio_print()

REGISTERS = [0x00000000] * (60 * 4)

class PLATFORM:
    GPIO_BASE = 0xFE200000
    GPIO_GPFSEL0 = 0 * 4
    GPIO_GPSET0 = 7 * 4
    GPIO_GPCLR0 = 10 * 4
    GPIO_GPLEV0 = 13 * 4
    GPIO_PUP_PDN_CNTRL_REG0 = 57 * 4
Platform = PLATFORM()

GPIO_INPUT = 0b000
GPIO_OUTPUT = 0b001
GPIO_ALT0 = 0b100
GPIO_ALT1 = 0b101
GPIO_ALT2 = 0b110
GPIO_ALT3 = 0b111
GPIO_ALT4 = 0b011
GPIO_ALT5 = 0b010

GPIO_PULL_NONE = 0b00
GPIO_PULL_UP = 0b01
GPIO_PULL_DOWN = 0b10
GPIO_PULL_BOTH = 0b11

def mmio_read(address):
    return REGISTERS[int(address)]

def mmio_write(address, value):
    REGISTERS[int(address)] = value
    if (address > Platform.GPIO_GPSET0) and (address < Platform.GPIO_GPCLR0):
        REGISTERS[int(Platform.GPIO_GPLEV0 + (address - Platform.GPIO_GPSET0))] |= value
    elif (address > Platform.GPIO_GPCLR0) and (address < Platform.GPIO_GPLEV0):
        REGISTERS[int(Platform.GPIO_GPLEV0 + (address - Platform.GPIO_GPCLR0))] &= ~value

def mmio_print():
    for i in range(0, len(REGISTERS), 4):
        print("[{:016X}]: {:08X} | {:032b}".format(Platform.GPIO_BASE + i, REGISTERS[i], REGISTERS[i]))

def gpio_setup(pin, alt, pull):
    bank = Platform.GPIO_GPFSEL0 + (0x4 * int(pin / 10))
    remainder = pin % 10
    value = mmio_read(bank)
    value &= ~(0b111 << (3 * remainder))
    value |= (alt << (3 * remainder))
    mmio_write(bank, value)
    print(hex(value))
    
    bank = Platform.GPIO_PUP_PDN_CNTRL_REG0 + (0x4 * int(pin / 16))
    remainder = pin % 16
    value = mmio_read(bank)
    value &= ~(0b11 << (2 * remainder))
    value |= (pull << (2 * remainder))
    mmio_write(bank, value)
    print(hex(value))

def gpio_read(pin):
    return (((mmio_read(Platform.GPIO_GPLEV0 + (0x4 * int(pin / 32))) >> (pin % 32)) & 1) == 1)

def gpio_write(pin):
    (mmio_write(Platform.GPIO_GPSET0 + (0x4 * int(pin / 32)), (1 << (pin % 32))))

def gpio_reset(pin):
    (mmio_write(Platform.GPIO_GPCLR0 + (0x4 * int(pin / 32)), (1 << (pin % 32))))

if __name__ == "__main__":
    main()
