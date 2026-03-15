#include<stm32f411xe.h>
#include<SysTick.h>


void systick_init(uint32_t delay_hz){
	SysTick->LOAD = (16000000 / delay_hz) - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = (7U<<0);

}

void SysTick_Handler(void){
	extern uint32_t current_task;
	extern uint32_t task_count;
	current_task++;
	if(current_task>=task_count){
		current_task = 0;
	}
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
