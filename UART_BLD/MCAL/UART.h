#ifndef _UART_H_
#define _UART_H_
#define F_CPU 8000000UL /* Define CPU Frequency e.g. here its 8MHz */
#include <avr/io.h>     /* Include AVR std. library file */
#include <avr/interrupt.h>
#include <stdio.h>      /* Include std. library file */
#include <util/delay.h> /* Include Delay header file */

#define F_CPU 8000000UL /* Define frequency here its 8MHz */
// #define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define MAX_LEGNTH_DATA PSPM_PAGESIZE

#define IDLE 0
#define RUNNING 1

void UART_init(long USART_BAUDRATE);
unsigned char UART_RxChar();
void UART_TxChar(char ch);
void UART_SetRxCallback(void (*callback)(uint8_t *PTR));

uint8_t RX_Buffer[255];
uint8_t Legnth;

#endif
