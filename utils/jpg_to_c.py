import os
import sys

def jpg_to_c(input_path):
    """
    Converts a JPG file to a C source file containing a byte array.

    :param input_path: Path to the input JPG file
    """
    try:
        if not os.path.exists(input_path):
            print(f"Error: The file '{input_path}' does not exist.")
            return

        if not input_path.lower().endswith('.jpg'):
            print("Error: The input file must be a JPG.")
            return

        with open(input_path, 'rb') as jpg_file:
            jpg_data = jpg_file.read()

        filename_without_ext = os.path.splitext(os.path.basename(input_path))[0]
        array_name = filename_without_ext.replace('-', '_').replace(' ', '_')
        output_path = os.path.join(os.getcwd(), filename_without_ext + '.c')

        with open(output_path, 'w') as c_file:
            c_file.write(f'// Generated from {input_path}\n')
            c_file.write(f'const unsigned char {array_name}_jpg[] = {{\n')
            for i, byte in enumerate(jpg_data):
                if i % 12 == 0:
                    c_file.write('    ')
                c_file.write(f'0x{byte:02X}, ')
                if (i + 1) % 12 == 0:
                    c_file.write('\n')
            c_file.write('\n};\n')
            c_file.write(f'const unsigned int {array_name}_jpg_len = sizeof({array_name}_jpg);\n')

        print(f"Successfully converted '{input_path}' to '{output_path}'.")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python jpg_to_c.py <path_to_jpg>")
        sys.exit(1)

    input_file_path = sys.argv[1]
    jpg_to_c(input_file_path)