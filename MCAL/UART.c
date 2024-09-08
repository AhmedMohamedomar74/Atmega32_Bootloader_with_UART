#include "UART.h"
static uint8_t Iterator = 0;
static uint8_t UART_state = IDLE;

// Global function pointer for the application callback
void (*uart_rx_callback)(uint8_t * PTR) = NULL;

void UART_init(long USART_BAUDRATE)
{
	UCSRB |= (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);	 /* Turn on transmission and reception */
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); /* Use 8-bit character sizes */
	UBRRL = BAUD_PRESCALE;								 /* Load lower 8-bits of the baud rate value */
	UBRRH = (BAUD_PRESCALE >> 8);						 /* Load upper 8-bits*/
	sei();
}

unsigned char UART_RxChar()
{
	while ((UCSRA & (1 << RXC)) == 0)
		;		  /* Wait till data is received */
	return (UDR); /* Return the byte*/
}

void UART_TxChar(char ch)
{
	while (!(UCSRA & (1 << UDRE)))
		; /* Wait for empty transmit buffer*/
	UDR = ch;
}

ISR(USART_RXC_vect)
{

	static uint8_t Legnth;
	if (UART_state == IDLE)
	{
		UART_state = RUNNING;
		Legnth = UDR;
	}
	else
	{
		if (Iterator < Legnth)
		{
			RX_Buffer[Iterator] = UDR;
			Iterator++;
		}
		else
		{
			UART_state = IDLE;
			Iterator = 0;
			uart_rx_callback(RX_Buffer);
		}
	}
}

// Function to register the application callback
void UART_SetRxCallback(void (*callback)(uint8_t *PTR))
{
	uart_rx_callback = callback;
}
