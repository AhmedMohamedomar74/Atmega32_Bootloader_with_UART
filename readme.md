# UART Bootloader with UDS Protocol for ATmega32

## Overview

This project implements a UART-based bootloader for the ATmega32 microcontroller, utilizing the Unified Diagnostic Services (UDS) protocol. The bootloader enables programming the target microcontroller via serial communication (UART) with a host system (e.g., a Python-based flashing unit), which sends the program to be stored in the flash memory of the ATmega32. The UDS protocol ensures reliable communication by defining a structured way to exchange data, handle requests, and manage error responses.

## Project Features

- **Communication Protocol**: Uses UART for data transfer between the host (flash unit) and target (ATmega32).
- **UDS Protocol Implementation**: The UDS protocol is applied on the ATmega32 to manage the flash memory programming process and handle requests.
- **Python Flashing Script**: The host system runs a Python script to send the program data in hex format, using UDS commands to control the flash memory.
- **Sequential Page Writing**: The program data is sent in chunks (pages), and each chunk is written to flash memory, with error checks at every stage.
- **Positive/Negative Responses**: The bootloader sends feedback to the host after each UDS command is processed, ensuring reliable communication.

## Communication Flow

### Flashing Unit (Python Script) to Target (ATmega32)

Below is a visual representation of how the flashing unit communicates with the ATmega32 via the UDS protocol.

![Communication Flow](Documentiation\Communication.png)

*This diagram illustrates the step-by-step communication and data exchange between the host and the ATmega32 target device.*

### ATmega32 Flowchart

The following flowchart details how the ATmega32 handles incoming UDS requests during the bootloading process:

![ATmega32 Flowchart](Documentiation\Flowchart.png)

*This flowchart shows the logic flow on the ATmega32, from validating requests to performing actions and sending responses.*

## Demo

Here’s a demo of the UART bootloader in action:

![Demo GIF](Documentiation\DEMO.gif)

*This GIF demonstrates the communication between the Python flashing script and the ATmega32 microcontroller during the flashing process.*

## How to Use

1. **Setup**: Connect the ATmega32 to the host system via UART.
2. **Python Script**: Run the Python script provided with the project to send the program data to the ATmega32 using UDS protocol commands.
3. **Monitor Responses**: Ensure the ATmega32 responds positively at each stage before proceeding to the next.
4. **Completion**: After the transfer is complete, the ATmega32 will perform a CRC check and jump to the newly flashed program.

## UDS Commands Used

- **0x10**: Trigger programming session.
- **0x34**: Request to download data (hex values).
- **0x36**: Transfer the actual data.
- **0x37**: End the transfer.
- **0x31**: CRC check to verify data integrity.

## Conclusion

This UART bootloader project provides a robust solution for updating the firmware of ATmega32 microcontrollers via UART using the UDS protocol. The Python script on the host side and the bootloader on the ATmega32 work together to ensure a reliable and error-free programming process.
