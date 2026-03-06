/*
 * scheduler.h
 *
 *  Created on: 05-Mar-2026
 *      Author: HARSHA
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include<stdint.h>

#define STACK_SIZE								256
#define MAX_TASKS								4

typedef enum{
	TASK_RUNNING,
	TASK_READY,
	TASK_BLOCKED,
	TASK_SUSPENDED
}task_state_t;

typedef struct{
	uint32_t stack_pointer;
	uint32_t stack[STACK_SIZE];
	task_state_t task_state;
	void (*task_function)(void);
}TCB_t;

#endif /* SCHEDULER_H_ */
