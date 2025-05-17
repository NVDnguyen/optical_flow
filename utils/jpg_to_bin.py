import os
import sys

def jpg_to_bin(input_path):
    """
    Converts a JPG file to a binary file with the same name.

    :param input_path: Path to the input JPG file
    """
    try:
        if not os.path.exists(input_path):
            print(f"Error: The file '{input_path}' does not exist.")
            return

        # Check if the input file is a JPG
        if not input_path.lower().endswith('.jpg'):
            print("Error: The input file must be a JPG.")
            return

        # Read the JPG file in binary mode
        with open(input_path, 'rb') as jpg_file:
            jpg_data = jpg_file.read()

        # Generate the output path
        filename_without_ext = os.path.splitext(os.path.basename(input_path))[0]
        current_dir = os.getcwd()
        output_path = os.path.join(current_dir, filename_without_ext + '.bin')

        # Write the binary data to the output file
        with open(output_path, 'wb') as bin_file:
            bin_file.write(jpg_data)

        print(f"Successfully converted '{input_path}' to '{output_path}'.")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python convert_jpg_to_bin.py <path_to_jpg>")
        sys.exit(1)

    input_file_path = sys.argv[1]
    jpg_to_bin(input_file_path)
