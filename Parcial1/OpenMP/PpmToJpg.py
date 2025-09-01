from PIL import Image
import sys

if len(sys.argv) != 3:
    print("Uso: python convert.py entrada.ppm salida.jpg")
    sys.exit(1)

entrada = sys.argv[1]
salida = sys.argv[2]


img = Image.open(entrada).convert("RGB")
img.save(salida, "JPEG")
