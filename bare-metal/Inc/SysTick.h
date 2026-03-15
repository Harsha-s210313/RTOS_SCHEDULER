/*
 * SysTick.h
 *
 *  Created on: 05-Mar-2026
 *      Author: HARSHA
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include<stdint.h>

void systick_init(uint32_t delay_hz);

void SysTick_Handler(void);

#endif /* SYSTICK_H_ */
