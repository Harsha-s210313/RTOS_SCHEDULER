/*
 * task.h
 *
 *  Created on: 05-Mar-2026
 *      Author: HARSHA
 */

#ifndef TASK_H_
#define TASK_H_

#include<stdint.h>

void task_create(void(*task_function)(void));
uint32_t task_get_current(void);

#endif /* TASK_H_ */
