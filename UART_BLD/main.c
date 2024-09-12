
#include "Application/Flashing_manger.h"

// int main()
// {

// 	Flashing_manger_init();
// 	Move_interrupts();
// 	while (1)
// 	{
// 		Flash_manger_unit();
// 	}
// }

/*
 * main.c
 *
 *  Created on: 11Sep.,2024
 *      Author: ahmed
 */

#include "MCAL/UART.h"
void operation(uint8_t *REC_DATA);
int main()
{
	UART_init(9600);
	UART_SetRxCallback(operation);
	Move_interrupts();
	while (1)
	{
	}
}

void operation(uint8_t *REC_DATA)
{
	uint8_t code_size = Legnth;
	static uint8_t First_time = 0;
	uint8_t *Code_arr = (uint8_t *)malloc((code_size * 2) * sizeof(uint8_t));

	if (First_time == 0)
	{
		for (uint8_t i = 0; i < code_size; i++)
		{
			Code_arr[i] = REC_DATA[i];
		}
		UART_TxChar(0x55);
		DDRA = Code_arr[code_size - 1];
		First_time = 1;
	}
	else
	{
		for (uint8_t i = 0; i < code_size; i++)
		{
			Code_arr[i + 128] = REC_DATA[i];
		}

		DDRB = Code_arr[(2 * code_size) - 1];
		UART_TxChar(0x65);
		write_buffer_to_flash(0x0000,Code_arr,256);

		jump_to_application();
	}
}
