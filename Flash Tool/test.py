import serial
import time

# Configure the serial port (adjust the port name and baudrate as needed)
ser = serial.Serial('COM1', 9600)  # Replace 'COM1' with your actual port (e.g., '/dev/ttyUSB0' for Linux)
# time.sleep(2)  # Wait for the serial connection to establish

# Define the message to send in hex format (0x1003)
message = bytes([0x10, 0x03])

# Get the length of the message in bytes (in this case, 2)
message_length = len(message)

# Send the length of the message as a single byte
ser.write(bytes([message_length]))

# Send the actual message (0x10 followed by 0x03)
ser.write(message)

# Send a null byte (0x00) to terminate the message
ser.write(bytes([0x00]))

# Close the serial port connection
ser.close()
