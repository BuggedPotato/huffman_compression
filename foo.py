lines = []
with open("input/bee-movie.txt", "r") as file:
    for line in file:
        if chr(35) in line:
            line = line.replace( chr(35), '' )
        lines.append(line)

with open("input/bee-movie-cleaned.txt", "w") as file:
    file.writelines(lines)
