import sys

def hex_to_array(file_path):
    program_data = []
    
    try:
        with open(file_path, 'r') as hex_file:
            for line in hex_file:
                if line[0] != ':':
                    continue  # Ignore lines that don't start with ':'
                
                # Parse the record length and data from each line
                byte_count = int(line[1:3], 16)
                address = int(line[3:7], 16)
                record_type = int(line[7:9], 16)
                
                if record_type == 0:  # Data record
                    data_start = 9
                    for i in range(byte_count):
                        byte = int(line[data_start:data_start+2], 16)
                        program_data.append(byte)
                        data_start += 2
                
                # Stop reading on an end-of-file (EOF) record (type 1)
                if record_type == 1:
                    break

        # Calculate how many bytes to add to make the array size a multiple of 128
        current_size = len(program_data)
        next_multiple_of_128 = ((current_size + 127) // 128) * 128  # Round up to the next multiple of 128

        # Calculate how many padding bytes (0xFF) are needed
        padding_needed = next_multiple_of_128 - current_size

        # Add 0xFF to pad the array to the correct size
        program_data.extend([0xFF] * padding_needed)

        # Convert the program data into a formatted string for C code
        formatted_data = ', '.join(f'0x{byte:02X}' for byte in program_data)
        
        # Display the C array format
        print("uint8_t program_data[] = {")
        print(f"    {formatted_data}")
        print("};")
        print(f"// Size of array: {len(program_data)} bytes (padded with {padding_needed} bytes)")

    except FileNotFoundError:
        print(f"Error: The file '{file_path}' was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage: python hex_to_array.py firmware.hex
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python hex_to_array.py <hex file>")
    else:
        hex_to_array(sys.argv[1])
