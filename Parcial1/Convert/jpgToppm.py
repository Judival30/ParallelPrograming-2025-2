from PIL import Image

# Abrir imagen JPG
img = Image.open(r"C:\Users\judiv\OneDrive\Escritorio\Universidad\Semestre_7\Paralela\Parcial1\1.jpg").convert("RGB")

N, M = img.size
mxVal = 255

with open("salida.ppm", "w") as f:
    f.write("P3\n")
    f.write(f"{N} {M}\n")
    f.write(f"{mxVal}\n")
    
    for y in range(M):
        for x in range(N):
            r, g, b = img.getpixel((x, y))
            f.write(f"{r} {g} {b} ")
        f.write("\n")
