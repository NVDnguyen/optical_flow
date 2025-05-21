import os
import sys
from PIL import Image

def rgb888_to_rgb565(r, g, b):
    # Chuyển 8-bit RGB sang 16-bit RGB565
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def jpg_to_rgb565_header(input_path):
    try:
        if not os.path.exists(input_path):
            print(f"Error: The file '{input_path}' does not exist.")
            return

        if not input_path.lower().endswith(('.jpg', '.jpeg')):
            print("Error: The input file must be a JPG or JPEG.")
            return

        # Mở ảnh bằng Pillow
        img = Image.open(input_path)
        img = img.convert('RGB')  # Chuyển sang RGB888

        width, height = img.size
        pixels = list(img.getdata())

        filename_without_ext = os.path.splitext(os.path.basename(input_path))[0]
        array_name = filename_without_ext.replace('-', '_').replace(' ', '_')
        output_path = os.path.join(os.getcwd(), filename_without_ext + '_rgb565.h')

        with open(output_path, 'w') as c_file:
            c_file.write(f'// Generated from {input_path}\n')
            c_file.write(f'#ifndef {array_name.upper()}_RGB565_H\n#define {array_name.upper()}_RGB565_H\n\n')
            c_file.write(f'const unsigned short {array_name}_rgb565[{width * height}] = {{\n')

            for i, (r, g, b) in enumerate(pixels):
                rgb565 = rgb888_to_rgb565(r, g, b)
                if i % 12 == 0:
                    c_file.write('    ')
                c_file.write(f'0x{rgb565:04X}, ')
                if (i + 1) % 12 == 0:
                    c_file.write('\n')

            c_file.write('\n};\n\n')
            c_file.write(f'const unsigned int {array_name}_rgb565_width = {width};\n')
            c_file.write(f'const unsigned int {array_name}_rgb565_height = {height};\n\n')
            c_file.write(f'#endif // {array_name.upper()}_RGB565_H\n')

        print(f"Successfully converted '{input_path}' to '{output_path}'.")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python jpg_to_rgb565_header.py <path_to_jpg>")
        sys.exit(1)

    input_file_path = sys.argv[1]
    jpg_to_rgb565_header(input_file_path)
