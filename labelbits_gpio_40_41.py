class label:
    def __init__(self, name = "[LABEL]",
                 shift = 0, mask = 0xFFFFFFFF, comment = "", *args, **kwargs):
        self.name = name
        self.shift = shift
        self.mask = mask
        self.comment = comment;

class labelbits:
    def __init__(self, labels = [], bits = 32, *args, **kwargs):
        self.bits = bits
        self.labels = labels

    def cover(self, binary, item):
        masked = ""
        mask = item.mask << item.shift
        for i in range(self.bits -1, -1, -1):
            if ((mask >> i) & 1):
                masked += str((binary >> i) & 1)
            else:
                masked += "-"
        return masked

    def mode(self, binary):
        mapping = ["Input", "Output", "ALT5", "ALT4", "ALT0", "ALT1", "ALT2", "ALT3"]
        if (binary >= 0) and (binary < len(mapping)):
            return mapping[binary]
        else:
            return str(binary)

    def info(self, binary, indent = 32):
        print("<<<LabelBits>>>")
        raw = format(binary, "#0" + str(self.bits + 2) + "b")[2:]
        print(("Raw Data:\t").expandtabs(indent) + str(raw) + " [LSB]\n")
        for item in self.labels:
            data = (binary >> item.shift) & item.mask
            line = str(item.name) + ":\t" + self.cover(binary, item)
            comment = "//" + str(item.comment)
            print(comment + "\n" + line.expandtabs(indent) + " [" +
                  self.mode(data) + "]\n")

ALT_LABELS = \
[
    label("FSEL40", 0, 0b111, "GPIO 40"),
    label("FSEL41", 3, 0b111, "GPIO 41"),
    label("FSEL42", 6, 0b111, "GPIO 42"),
    label("FSEL43", 9, 0b111, "GPIO 43"),
    label("FSEL44", 12, 0b111, "GPIO 44"),
    label("FSEL45", 15, 0b111, "GPIO 45"),
    label("FSEL46", 18, 0b111, "GPIO 46"),
    label("FSEL47", 21, 0b111, "GPIO 47"),
    label("FSEL48", 24, 0b111, "GPIO 48"),
    label("FSEL49", 27, 0b111, "GPIO 49"),
    label("-", 30, 0b11, "Reserved"),
]

print("000 - Input")
print("001 - Output")
print("100 - ALT0")
print("101 - ALT1")
print("110 - ALT2")
print("111 - ALT3")
print("011 - ALT4")
print("010 - ALT5")
print()
GPIO = labelbits(ALT_LABELS)
#GPIO.info(0x00000064)
#GPIO.info(0x0000006D)
GPIO.info(0x00000009)
