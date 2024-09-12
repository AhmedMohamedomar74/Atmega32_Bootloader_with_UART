/*
 * Flashing_manger.h
 *
 *  Created on: 8Sep.,2024
 *      Author: ahmed
 */

#ifndef APPLICATION_FLASHING_MANGER_H_
#define APPLICATION_FLASHING_MANGER_H_

#include "../MCAL/UART.h"
#include "../HAL/LCD.h"
#include <avr/boot.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/crc16.h>


#define SESSION_CONTROL (0X10)
#define PROGRAMMING_SESSION (0x03)
#define DOWNLOAD_REQUEST (0X34)
#define TRANSFER_DATA (0X36)
#define TRANSFER_EXIT (0X37)
#define CHECK_CRC (0X31)

typedef enum
{
    waiting_ProgrammingSession,
    waiting_DownloadRequest,
    waiting_TransferData,
    waiting_TransferExit,
    waiting_CheckCRC
} downloadStates_t;

#define PAGE_SIZE 128

void Flashing_manger_init();

void REQ_notification(uint8_t *REQ);

void Flash_manger_unit();


uint8_t LOC_vidCheckFlashCRC(uint16_t u16StartAdd, uint16_t u16EndAdd, uint16_t u16CRC);

void Move_interrupts(void);

void Move_interrupts_to_application(void);

void boot_program_page(uint32_t page, uint8_t *buf);

void write_buffer_to_flash(uint32_t start_page, uint8_t *data_buffer, uint32_t buffer_size);


uint8_t Actual_code[256];

#endif /* APPLICATION_FLASHING_MANGER_H_ */
