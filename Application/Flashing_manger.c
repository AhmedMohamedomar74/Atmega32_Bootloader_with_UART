#include "Flashing_manger.h"
/*
 * Flashing_manger.c
 *
 *  Created on: 8Sep.,2024
 *      Author: ahmed
 */

void REQ_notification(uint8_t *REQ)
{
    switch (REQ[0])
    {
    case 0x10:
        LCD_String("Triggered PROGRAM");
        break;

    default:
        break;
    }
}

void Flashing_manger_init()
{
    UART_init(9600);
	UART_SetRxCallback(REQ_notification);
    LCD_Init();
}
