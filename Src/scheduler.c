#include<scheduler.h>

void PendSV_Handler(void){
	extern uint32_t current_task;
	extern uint32_t task_count;
	extern TCB_t tasks[MAX_TASKS];
	uint32_t psp;
	__asm volatile("MRS %0, PSP"
			: "=r"(psp));
	__asm volatile("STMDB %0!, {R4-R11}"
			: "+r"(psp));

	uint32_t prev_task = (current_task == 0) ? (task_count - 1): (current_task - 1);
	tasks[prev_task].stack_pointer = psp;
	psp = tasks[current_task].stack_pointer;
	__asm volatile("LDMIA %0!, {R4-R11}"
			: "+r"(psp));
	__asm volatile("MSR PSP, %0"
			: : "r"(psp));

}
