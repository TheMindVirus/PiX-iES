def main():
    #source = "../build/BOOT.o"
    #source = "../build/iES.o"
    #source = "../build/PiX.o"
    #source = "../build/PiX.elf"
    #source = "../build/PiX.img"
    source = "../build/PiX.iES"

    file = open(source, "rb")
    data = file.read()
    file.close()

    term = b"PiX" #GNU 
    test = True #False

    tmp = [data, term, source]
    
    if test:
        data = b"Hello World"
        term = b"l"
        source = data

    mask = bytes2bits(b"\xFF")
    bits = bytes2bits(data)
    look = bytes2bits(term)
    nice = bytes2bits(b"\x69")

    print("[INFO]: Searching for {} in {}...".format(term, source))

    result = bitxor(bits, bitback(nice), (0, 0), (3 + 8, len(bits)))
    print(result)
    result = bits2bytes(result)
    print(result)
    result = hexpack(result)
    print(result)
    
    result = bitclr(bits, nice, (0, 0), (0 + 0, len(bits)))
    carry = result
    print(result)
    result = bits2bytes(result)
    print(result)
    result = hexpack(result)
    print(result)
    
    result = bitset(carry, nice, (0, 0), (0 + 0, len(carry)))
    print(result)
    result = bits2bytes(result)
    print(result)
    result = hexpack(result)
    print(result)

    data, term, source = tmp
    print("[INFO]: Searching for {} in {}...".format(term, source))

    mask = bytes2bits(b"\xFF")
    bits = bytes2bits(data, "big")
    look = bytes2bits(term, "big")
    nice = bytes2bits(b"\x69")
    
    result = bitfind(bits, look)
    print(*hexlist(result), sep = "\n")
    result = bitfind(bitback(bits), bitback(look))
    print(*hexlist(result, backwards = True), sep = "\n")

    mask = bytes2bits(b"\xFF")
    bits = bytes2bits(data, "little")
    look = bytes2bits(term, "little")
    nice = bytes2bits(b"\x69")
    
    result = bitfind(bits, look)
    print(*hexlist(result, endian = "little"), sep = "\n")
    result = bitfind(bitback(bits), bitback(look))
    print(*hexlist(result, backwards = True, endian = "little"), sep = "\n")
    
    print("[INFO]: Done!")

def bitrange(stream, start, end):
    return stream[max(start, 0):min(end, len(stream))]

def bytes2bits(data, endian = "big"):
    stream = ""
    if endian == "little":
        for i in range(len(data)-1,-1,-1):
            stream += "{:08b}".format(data[i])
    else:
        for i in range(0, len(data)):
            stream += "{:08b}".format(data[i])
    return stream

def bits2bytes(stream, endian = "big"):
    data = []
    length = len(stream)
    if endian == "little":
        for i in range(length, 0, -8):
            br = bitrange(stream,i-8,i)
            data.append(int("0b{}".format(br) if br != "" else "0b0", 2))
    else:
        for i in range(0, length, 8):
            br = bitrange(stream,i,i+8)
            data.append(int("0b{}".format(br) if br != "" else "0b0", 2))
    return bytes(data)

def bitscope(stream, left = 0, right = 0, swap = False):
    if swap:
        left, right = [right, left]
    if left > 0:
        stream = ("0" * left) + stream
    if right > 0:
        stream = stream + ("0" * right)
    if left < 0:
        stream = stream[max(-left, 0):]
    if right < 0:
        stream = stream[:min(len(stream)+right, len(stream))]
    return stream

def bitxor(stream = "", value = "", scope_stream = (0, 0), scope_value = (0, 0)):
    stream = bitscope(stream, *scope_stream)
    value = bitscope(value, *scope_value)
    newstream = ""
    for i in range(0, len(stream)):
        newstream += "1" if stream[i] != value[i] else "0"
    return newstream

def bitclr(stream = "", value = "", scope_stream = (0, 0), scope_value = (0, 0)):
    stream = bitscope(stream, *scope_stream)
    value = bitscope(value, *scope_value)
    newstream = ""
    for i in range(0, len(stream)):
        newstream += "0" if value[i] == "1" else stream[i]
    return newstream

def bitset(stream = "", value = "", scope_stream = (0, 0), scope_value = (0, 0)):
    stream = bitscope(stream, *scope_stream)
    value = bitscope(value, *scope_value)
    newstream = ""
    for i in range(0, len(stream)):
        newstream += "1" if value[i] == "1" else stream[i]
    return newstream

def bitback(stream):
    return "".join([stream[i] for i in range(len(stream)-1,-1,-1)])

def bitfind(stream = "", value = ""):
    results = []
    pos = stream.find(value)
    while pos > -1:
        if pos > -1:
            results.append(pos)
            pos = stream.find(value, pos + 1)
    return results

def hexlist(records, backwards = False, endian = "big"):
    result = []
    for record in records:
        result.append(
        {
            "location": record,
            "address": hex(int(record / 8)),
            "shift": (record - (int(record / 8) * 8)) * (1 if not backwards else -1),
            "backwards": backwards,
            "endian": endian,
        })
    if result == []:
        result.append(
        {
            "location": -1,
            "address": hex(0),
            "shift": 0,
            "backwards": backwards,
            "endian": endian,
        })
    return result

def hexpack(pack, mode = 0):
    if mode == 1:
        raw = b""
        for i in range(0, len(pack)):
            raw += pack[i].to_bytes(1, "big")
    else:
        raw = ""
        for i in range(0, len(pack)):
            raw += "{:02X} ".format(pack[i])
    return raw

if __name__ == "__main__":
    main()
