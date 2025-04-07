import struct
import argparse
import os

# Define the set of allowed Polish diacritics and ASCII characters
ALLOWED_CHARACTERS = set(
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZćęłńóśźż0123456789"
)

def write_wchar_to_file(filename):
    # Check if the file already exists
    if not os.path.exists(filename):
        print("File does not exist. Scan your card while unmounted and then remount.")
        return

    # Get the input string from the user
    input_string = input("Enter name (max 12 characters): ")

    if len(input_string) > 12:
        print("Input string exceeds 12 characters.")
        return

    # Check for any invalid characters
    invalid_characters = [c for c in input_string if c not in ALLOWED_CHARACTERS]
    
    if len(invalid_characters):
        print(f"Invalid characters detected: {', '.join(invalid_characters)}")
        return

    # Ensure the string is exactly 12 characters long
    # Pad with null characters if the string is shorter than 12 characters
    padded_string = input_string.ljust(12, '\0')

    # Convert the string to a list of wchar_t (in Python, we use 4 bytes per wchar_t, typically UTF-32)
    wchar_array = [ord(c) for c in padded_string]

    # Pack the wchar_t array into a byte string (12 characters * 4 bytes each)
    byte_data = struct.pack('12I', *wchar_array)  # 12 wchar_t values (4 bytes each, 'I' for unsigned int)

    # Write the data to the file
    with open(filename, 'wb') as file:
        file.write(byte_data)

    print(f"Name written to {filename} successfully.")

def main():
    # Set up argument parser to get file name from command line
    parser = argparse.ArgumentParser(description="Write a name as wchar_t[] to a file.")
    parser.add_argument("filename", help="The name of the output file")

    # Parse the command-line arguments
    args = parser.parse_args()

    # Call the function to write the string to the specified file
    write_wchar_to_file(args.filename)

if __name__ == "__main__":
    main()
