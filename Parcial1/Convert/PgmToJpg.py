from PIL import Image
import sys

if len(sys.argv) != 3:
    print("Uso: python pgmTojpg.py entrada.pgm salida.jpg")
    sys.exit(1)

entrada = sys.argv[1]
salida = sys.argv[2]


img = Image.open(entrada)
img.save(salida)
