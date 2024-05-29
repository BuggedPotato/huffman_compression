res = [0] * 128
with open("ascii_freq.txt", "r") as file:
    for line in file:
        tmp = line.split(":")
        el = (int(tmp[0]), float(tmp[1]))
        res[el[0]] = el[1]

with open("ascii_freq_ARRAY.txt", "w") as file:
    file.write("{\n")
    for v in res:
        file.write(str(v))
        file.write(",\n")
    file.write("}")
