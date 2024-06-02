with open("output.bin", "rb") as file:
    bytes = file.read()
    for i in range(len(bytes)):
        print("{1}: {0:b}".format(bytes[i], i))
