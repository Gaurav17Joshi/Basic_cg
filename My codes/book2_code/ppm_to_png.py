from PIL import Image
import sys


def ppm_to_png(ppm_file_path, png_file_path):
    try:
        # Open the PPM file
        with Image.open(ppm_file_path) as img:
            # Save as PNG
            img.save(png_file_path, "PNG")
        print(f"Successfully converted '{ppm_file_path}' to '{png_file_path}'")
    except FileNotFoundError:
        print(f"Error: PPM file not found at '{ppm_file_path}'")
    except Exception as e:
        print(f"An error occurred during conversion: {e}")

if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("Usage: python ppm_to_png.py <input.ppm> <output.png>")
        sys.exit(1)

    ppm_file = sys.argv[1]
    png_file = sys.argv[2]
    ppm_to_png(ppm_file, png_file)