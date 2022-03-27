STATE_GPIO = 0

def gpio_setup_old(pin, value, base, size):
    global STATE_GPIO
    STATE_GPIO = 0
    mask = (1 << size) - 1
    if pin > 58: #53
        STATE_GPIO = 2
        return
    if value > mask:
        STATE_GPIO = 3
        return
    
    fields = int(32 / size)
    address = base + (int(pin / fields) * 4)
    shift = (pin % fields) * size

    edit = 0 # mmio_read
    #edit = 0xDEADBEEF
    edit &= ~(mask << shift)
    edit |= (value << shift)
    print("{:016x} | {:08x} | {:032b}".format(address, edit, edit)) # mmio_write
    #print("{:016x} | {:08x} | {:032b}".format(address, edit, edit & ~0xDEADBEEF)) # mmio_write
    STATE_GPIO = 1

gpio_setup_old(40, 0b00, 0xFE2000E4, 2)
print(STATE_GPIO)
gpio_setup_old(41, 0b00, 0xFE2000E4, 2)
print(STATE_GPIO)
gpio_setup_old(40, 0b010, 0xFE200000, 3)
print(STATE_GPIO)
gpio_setup_old(41, 0b010, 0xFE200000, 3)
print(STATE_GPIO)
gpio_setup_old(40, 1, 0xFE20001C, 1)
print(STATE_GPIO)
gpio_setup_old(41, 1, 0xFE20001C, 1)
print(STATE_GPIO)
gpio_setup_old(40, 1, 0xFE200028, 1)
print(STATE_GPIO)
gpio_setup_old(41, 1, 0xFE200028, 1)
print(STATE_GPIO)

# 1) Pull-ups in REG2 set to PULL_NONE
# 2) Alt-mode in FSEL4 set to ALT5 for Pins 40 and 41
# 3) Set Bits in SET1 set to 1 for Pins 40 and 41
# 4) Clear Bits in CLR1 set to 1 for Pins 40 and 41

"""
00000000fe2000ec | 00000000 | 00000000000000000000000000000000
1
00000000fe2000ec | 00000000 | 00000000000000000000000000000000
1
00000000fe200010 | 00000002 | 00000000000000000000000000000010
1
00000000fe200010 | 00000010 | 00000000000000000000000000010000
1
00000000fe200020 | 00000100 | 00000000000000000000000100000000
1
00000000fe200020 | 00000200 | 00000000000000000000001000000000
1
00000000fe20002c | 00000100 | 00000000000000000000000100000000
1
00000000fe20002c | 00000200 | 00000000000000000000001000000000
1
"""

"""
00000000fe2000ec | deacbeef | 00000000000000000000000000000000
1
00000000fe2000ec | dea1beef | 00000000000000000000000000000000
1
00000000fe200010 | deadbeea | 00000000000000000000000000000000
1
00000000fe200010 | deadbed7 | 00000000000000000000000000010000
1
00000000fe200020 | deadbfef | 00000000000000000000000100000000
1
00000000fe200020 | deadbeef | 00000000000000000000000000000000
1
00000000fe20002c | deadbfef | 00000000000000000000000100000000
1
00000000fe20002c | deadbeef | 00000000000000000000000000000000
1
"""
