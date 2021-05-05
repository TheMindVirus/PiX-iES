import sys, time
from capstone import *
from capstone.arm import *

def fetch(path):
    file = open(path, "rb")
    data = file.read()
    print("Bytes Read: " + str(len(data)))
    file.close()
    return data

def decode(arch, code, offset = 0x0, length = 0):
    arch = arch.lower().strip()
    result = ""
    if(arch == "x86"):
        print("__x86_Disassembly__")
        disassembler = Cs(CS_ARCH_X86, CS_MODE_32)
    elif(arch == "x64"):
        print("__x64_Disassembly__")
        disassembler = Cs(CS_ARCH_X86, CS_MODE_64)
    elif(arch == "arm"):
        print("__ARM_Disassembly__")
        disassembler = Cs(CS_ARCH_ARM, CS_MODE_ARM)
    elif(arch == "arm64"):
        print("__ARM64_Disassembly__")
        disassembler = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    else:
        print("Unknown Architecture Specified", file = sys.stderr)
        return result
    if (length == 0):
        code = code[offset:]
    else:
        code = code[offset:offset + (length * 4)]
    disassembly = disassembler.disasm(code, offset)
    for step in disassembly:
        line = format("0x%x:\t%d : %s %s" %(step.address, step.size, step.mnemonic, step.op_str))
        result += format(line) + "\n"
        for char in line:
            sys.stdout.write(char)
            time.sleep(0.001)
        sys.stdout.write("\n")
    return result

if (__name__ == "__main__"):
    #decode("arm64", fetch("PiX.iES"), 0x80000)
    #decode("arm64", fetch("PiX.iES"), 0x8000)
    decode("arm64", fetch("PiX.iES"), 0x25ec)
    #decode("arm64", fetch("PiX.iES"), 0xf30)
    #decode("arm64", fetch("PiX.iES"), 0x800, 10)
    #decode("arm64", fetch("PiX.iES"), 0x800)
