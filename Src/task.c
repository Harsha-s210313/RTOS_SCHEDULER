
#include<task.h>

#include<scheduler.h>

TCB_t tasks[MAX_TASKS];
uint32_t task_count = 0;
uint32_t current_task = 0;
void task_create(void(*task_function)(void)){
	uint32_t *sp = &tasks[task_count].stack[STACK_SIZE - 1];
	*(sp) = (0x01000000);
	*(--sp) = (uint32_t) task_function;
	*(--sp) = 0;
	*(--sp) = 0;
	*(--sp) = 0;
	*(--sp) = 0;
	*(--sp) = 0;
	*(--sp) = 0;
	tasks[task_count].stack_pointer = *sp;
	tasks[task_count].task_state = TASK_READY;
	task_count++;
}

uint32_t task_get_current(void){
	return current_task;
}
