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

void Flashing_manger_init();

void REQ_notification(uint8_t *REQ);

#endif /* APPLICATION_FLASHING_MANGER_H_ */
