#include "Flashing_manger.h"
/*
 * Flashing_manger.c
 *
 *  Created on: 8Sep.,2024
 *      Author: ahmed
 */
static uint8_t REC_REQ = 0;
uint8_t *Actual_req = NULL;
static downloadStates_t download_state = waiting_ProgrammingSession;
uint8_t Actual_legnth;
uint16_t code_size;
static uint32_t page_counter = 0;

extern void
send_Positve_response();
extern void send_Negaitve_response();
void REQ_notification(uint8_t *REQ)
{
    REC_REQ = 1;
    Actual_req = REQ;
    Actual_legnth = Legnth;
}

void Flash_manger_unit()
{
    if (REC_REQ == 1)
    {
        REC_REQ = 0;
        switch (Actual_req[0])
        {
        case SESSION_CONTROL:
            if ((Actual_req[1] == PROGRAMMING_SESSION) && (download_state == waiting_ProgrammingSession) && (Actual_legnth == 2))
            {
                download_state = waiting_DownloadRequest;
                send_Positve_response();
                LCD_Clear();
                LCD_String("Programming session");
            }
            else
            {
                LCD_Clear();
                LCD_String("Error Prog session");
                download_state = waiting_ProgrammingSession;
                send_Negaitve_response();
            }
            break;
        case DOWNLOAD_REQUEST:
            if ((Actual_legnth >= 0x03) && (download_state == waiting_DownloadRequest))
            {
                send_Positve_response();
                download_state = waiting_TransferData;
                LCD_Clear();
                LCD_String("Download REQ R");

                // Convert it to big-endian
                code_size = Actual_req[2] << 8 | Actual_req[3];
            }
            else
            {
                download_state = waiting_DownloadRequest;
                LCD_Clear();
                LCD_String("Download REQ F");
                send_Negaitve_response();
            }
            break;
        case TRANSFER_DATA:
            if ((download_state == waiting_TransferData) && (Actual_legnth == (SPM_PAGESIZE + 1)))
            {
                boot_program_page(page_counter, &Actual_req[1]);
                LCD_Clear();
                LCD_String("TRANSFER_DATA R");
                send_Positve_response();
                page_counter++;
                if ((page_counter * SPM_PAGESIZE) == (code_size))
                {
                    download_state = waiting_TransferExit;
                }
                else
                {
                    download_state = waiting_TransferData;
                }
            }
            else
            {
                download_state = waiting_TransferData;
                LCD_Clear();
                LCD_String("TRANSFER_DATA F");
                send_Negaitve_response();
            }
            break;
        case TRANSFER_EXIT:
            if ((download_state == waiting_TransferExit) && (Actual_legnth == 1))
            {
                send_Positve_response();
                LCD_Clear();
                LCD_String("TRANSFER_EXIT R");
                download_state = waiting_CheckCRC;
            }
            else
            {
                send_Negaitve_response();
                LCD_Clear();
                LCD_String("TRANSFER_EXIT F");
                download_state = waiting_TransferExit;
            }
            break;
        case CHECK_CRC:
            if ((download_state == waiting_CheckCRC) && (Actual_legnth == 3))
            {
                DDRA = Actual_req[1];
                DDRC = Actual_req[2];
                uint16_t CRC_REC = Actual_req[1] << 8 | Actual_req[2];
                uint8_t CRC_CHECK = LOC_vidCheckFlashCRC(0, code_size, CRC_REC);
                DDRA = CRC_CHECK;
                send_Positve_response();
                LCD_Clear();
                LCD_String("CHECK_CRC R");
                download_state = waiting_DownloadRequest;
            }
            else
            {
                send_Negaitve_response();
                LCD_Clear();
                LCD_String("CHECK_CRC F");
                download_state = waiting_CheckCRC;
            }
            break;
        default:
            break;
        }
    }
}



void write_buffer_to_flash(uint32_t start_page, uint8_t *data_buffer, uint32_t buffer_size)
{
    uint32_t num_pages = (buffer_size + PAGE_SIZE - 1) / PAGE_SIZE;  // Calculate number of pages needed
    uint32_t current_page_address = start_page;

    for (uint32_t page_num = 0; page_num < num_pages; page_num++)
    {
        // Write the current 128-byte chunk to the flash page
        boot_program_page(current_page_address, data_buffer + (page_num * PAGE_SIZE));

        // Move to the next flash page
        current_page_address += PAGE_SIZE;
    }
}


void boot_program_page(uint32_t page, uint8_t *buf)
{
    uint16_t i;
    uint8_t sreg;

    // Disable interrupts
    sreg = SREG;
    cli();

    eeprom_busy_wait();

    boot_page_erase(page);
    boot_spm_busy_wait(); // Wait until the memory is erased

    for (i = 0; i < PAGE_SIZE; i += 2)
    {
        // Set up little-endian word
        uint16_t w = *buf++;
        w += (*buf++) << 8;

        boot_page_fill(page + i, w);
    }

    boot_page_write(page); // Store buffer in flash page
    boot_spm_busy_wait();  // Wait until the memory is written

    // Re-enable the RWW-section again
    boot_rww_enable();

    // Re-enable interrupts
    SREG = sreg;
}

void Flashing_manger_init()
{
    UART_init(9600);
    UART_SetRxCallback(REQ_notification);
    LCD_Init();
}

extern void send_Positve_response()
{
    UART_TxChar((Actual_req[0] + 0x40));
}

extern void send_Negaitve_response()
{
    UART_TxChar(0x7f);
}

uint8_t LOC_vidCheckFlashCRC(uint16_t u16StartAdd, uint16_t u16EndAdd, uint16_t u16CRC)
{
    uint16_t addr;
    uint16_t WORD;
    uint16_t CRC16 = 0xFFFF;

    /* Compute the CRC */
    for (addr = u16StartAdd; addr < u16EndAdd; addr += 2)  // Increment by 2 to read word (2 bytes)
    {
        WORD = pgm_read_word(addr);  // Read 16-bit word from memory
        CRC16 = _crc16_update(CRC16, (WORD & 0xFF));       // Pass lower byte to CRC function
        CRC16 = _crc16_update(CRC16, ((WORD >> 8) & 0xFF)); // Pass upper byte to CRC function
    }

    /* Compare calculated CRC with received one */
    if (u16CRC != CRC16)
    {
        return 0; /* Bad CRC */
    }
    else
    {
        return 1; /* Good CRC */
    }
}

void Move_interrupts(void)
{
	/* Enable change of interrupt vectors */
	GICR = (1<<IVCE);
	/* Move interrupts to boot Flash section */
	GICR = (1<<IVSEL);
}

void Move_interrupts_to_application(void)
{
    /* Enable change of interrupt vectors */
    GICR = (1<<IVCE);
    /* Move interrupts to the application Flash section (reset to 0x0000) */
    GICR = (0<<IVSEL);  // Clear IVSEL to move interrupts to the application
}


void jump_to_application(void)
{
    // Disable interrupts before jumping
    cli();

    // Define a function pointer to the start address of the application (0x0000)
    void (*application_start)(void) = (void (*)(void))0x0000;

    // Jump to the application (0x0000)
    application_start();
}

