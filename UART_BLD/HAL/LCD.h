/*
 * LCD.h
 *
 *  Created on: 8Sep.,2024
 *      Author: ahmed
 */

#ifndef HAL_LCD_H_
#define HAL_LCD_H_

#define F_CPU 8000000UL			/* Define CPU Frequency e.g. here 8MHz */
#include <avr/io.h>			/* Include AVR std. library file */
#include <util/delay.h>			/* Include Delay header file */


#define LCD_Dir  DDRB			/* Define LCD data port direction */
#define LCD_Port PORTB			/* Define LCD data port */
#define RS PB0				/* Define Register Select pin */
#define EN PB1 				/* Define Enable signal pin */

void LCD_Command( unsigned char cmnd );
void LCD_Char( unsigned char data );
void LCD_Init (void);
void LCD_String (char *str)	;
void LCD_String_xy (char row, char pos, char *str);
void LCD_Clear();

#endif /* HAL_LCD_H_ */
