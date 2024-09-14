import serial
import time
import sys

# Function from hex_converter.py
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

        return program_data

    except FileNotFoundError:
        print(f"Error: The file '{file_path}' was not found.")
        return None
    except Exception as e:
        print(f"An error occurred: {e}")
        return None

# Initialize the serial connection (replace with your settings)
ser = serial.Serial('COM8', 9600, timeout=1)

# Example usage: python merged_script.py firmware.hex
if len(sys.argv) < 2:
    print("Usage: python merged_script.py <hex file>")
    sys.exit(1)

# Get program_data by reading the hex file
program_data = hex_to_array(sys.argv[1])
if program_data is None:
    sys.exit(1)

# Calculate the size of the array
program_size = len(program_data)
print(f"Size of program = {program_size}")

# Convert program_size to little-endian byte array
program_size_bytes = program_size.to_bytes((program_size.bit_length() + 7) // 8, 'big')

print(f"Length of program = {program_size_bytes}")

def calculate_crc(data):
    crc = 0xFFFF  # Initialize CRC to 0xFFFF
    polynomial = 0xA001  # Polynomial used in C code

    for byte in data:
        crc ^= byte  # XOR byte into CRC
        for _ in range(8):
            if crc & 1:  # Check if the lowest bit is set
                crc = (crc >> 1) ^ polynomial  # Shift right and XOR with polynomial
            else:
                crc >>= 1  # Just shift right
        crc &= 0xFFFF  # Ensure CRC remains 16-bit

    # Return CRC as a single 2-byte variable in little-endian order
    high_byte = crc & 0xFF  # Low byte
    low_byte = (crc >> 8) & 0xFF  # High byte
    return low_byte, high_byte  # Return bytes separately

CRC_high, CRC_low = calculate_crc(program_data)
print(f"CRC High Byte: {CRC_high}, CRC Low Byte: {CRC_low}")

# States with CRC bytes properly handled
states = [
    bytes([0x10, 0x03]),  # SESSION_CONTROL
    bytes([0x34, 0x00] + list(program_size_bytes)),  # DOWNLOAD_REQUEST with size in little-endian
    bytes([0x37]),  # TRANSFER_EXIT
    bytes([0x31, CRC_high, CRC_low])   # CHECK_CRC with CRC bytes
]

def send_state_with_response(state):
    # Send the length of the state (1 byte)
    state_length = len(state)
    ser.write(bytes([state_length]))

    # Send the actual state
    ser.write(state)
    time.sleep(1)  # Wait 1 second between states, as per your requirement

    # Read the response (1 byte)
    response = ser.read(1)
    if response:
        expected_response = state[0] + 0x40
        if response[0] == expected_response:
            print(f"Received valid response: {response.hex()}")
            return True
        else:
            print(f"Invalid response: {response.hex()}, expected: {hex(expected_response)}")
            return False
    else:
        print("No response received.")
        return False

def send_transfer_data():
    chunk_size = 128  # Size of each chunk
    total_size = len(program_data)
    index = 0

    while index < total_size:
        chunk = program_data[index:index + chunk_size]

        # If chunk is less than 128 bytes, pad with 0xFF
        if len(chunk) < chunk_size:
            chunk += [0xFF] * (chunk_size - len(chunk))

        # Create the transfer data packet (0x36 + 128 bytes)
        transfer_state = bytes([0x36]) + bytes(chunk)
        
        print(f"Sending chunk from index {index} to {index + chunk_size}")
        
        # Send the transfer state and check response
        if not send_state_with_response(transfer_state):
            print(f"Error in transferring chunk starting at index {index}. Retrying.")
            continue  # Retry sending the same chunk if an error occurs

        # Move to the next chunk
        index += chunk_size

def send_all_states():
    # Send initial states (SESSION_CONTROL and DOWNLOAD_REQUEST)
    for state in states[:-2]:
        if not send_state_with_response(state):
            print("Error occurred. Stopping.")
            return

    # Handle the TRANSFER_DATA state by sending chunks
    send_transfer_data()

    # Send remaining states (TRANSFER_EXIT, CHECK_CRC)
    for state in states[-2:]:
        if not send_state_with_response(state):
            print("Error occurred. Stopping.")
            break

# Send all the states
send_all_states()

# Close the serial connection
ser.close()
