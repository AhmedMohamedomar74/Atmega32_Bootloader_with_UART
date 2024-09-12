


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
#include "Application/Flashing_manger.h"
#include "MCAL/UART.h"
void operation(uint8_t *REC_DATA);
uint8_t * Glopal_PTR = NULL;
uint8_t Code_Arr[256];
uint8_t Flash_flag = 0;
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

	if (First_time == 0)
	{
		for (uint8_t i = 0; i < code_size; i++)
		{
			Code_Arr[i] = REC_DATA[i];
		}
		UART_TxChar(0x55);
		DDRA = Code_Arr[code_size - 1];
		First_time = 1;
	}
	else
	{
		for (uint8_t i = 0; i < code_size; i++)
		{
			Code_Arr[i + code_size] = REC_DATA[i];
		}

		DDRB = Code_Arr[(2 * code_size) - 1];
		UART_TxChar(0x65);
		Flash_flag = 1;


		write_buffer_to_flash(0x0000,Code_Arr,sizeof(Code_Arr));
		Move_interrupts_to_application();
		_delay_ms(100) ;
		/*start the actual program*/
		asm("jmp 0");
	}
}
