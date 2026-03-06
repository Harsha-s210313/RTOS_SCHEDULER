#include <stm32f411xe.h>
#include<SysTick.h>
#include<scheduler.h>
#include<task.h>

#define GPIOAEN     (1U<<0)
#define GPIOCEN     (1U<<2)
#define USART2EN    (1U<<17)

#define USART_TE    (1U<<3)
#define USART_UE    (1U<<13)
#define USART_TXE   (1U<<7)

void uart_init(void);
void uart_write(char ch);
void uart_send_string(char *str);

extern TCB_t tasks[MAX_TASKS];

void task1(void)
{
    RCC->AHB1ENR |= GPIOAEN;

    GPIOA->MODER &= ~(3U << (5*2));
    GPIOA->MODER |=  (1U << (5*2));

    while(1)
    {
        GPIOA->ODR ^= (1U<<5);

        for(volatile int i=0; i<10000000; i++);
    }
}


void task2(void)
{
    uart_init();

    while(1)
    {
        uart_send_string("HELLO PC. THIS IS STM32F411RE\r\n");

        for(volatile int i=0; i<5000000; i++);
    }
}

void task3(void)
{
    volatile uint32_t result = 0;
    volatile uint32_t i;

    while(1)
    {
        for(i = 0; i < 1000000; i++)
        {
            result += i;
            result ^= (result << 3);
        }
    }
}

void task4(void)
{
    RCC->AHB1ENR |= GPIOCEN;

    GPIOC->MODER &= ~(3U << (13*2));

    while(1)
    {
        if(!(GPIOC->IDR & (1U<<13)))
        {
            GPIOA->ODR ^= (1U<<5);

            for(volatile int i=0; i<500000; i++);
        }
    }
}

void uart_init(void)
{
    RCC->AHB1ENR |= GPIOAEN;
    RCC->APB1ENR |= USART2EN;

    /* PA2 -> Alternate function */
    GPIOA->MODER &= ~(3U << (2*2));
    GPIOA->MODER |=  (2U << (2*2));

    /* AF7 for USART2 */
    GPIOA->AFR[0] &= ~(0xF << 8);
    GPIOA->AFR[0] |=  (7U << 8);

    USART2->BRR = 0x0683;

    USART2->CR1 = USART_TE | USART_UE;
}

void uart_write(char ch)
{
    while(!(USART2->SR & USART_TXE)){}
    USART2->DR = ch;
}

void uart_send_string(char *str)
{
    while(*str)
    {
        uart_write(*str++);
    }
}
int main(void){
	task_create(task1);
	task_create(task2);
	task_create(task3);
	task_create(task4);

	systick_init(1000);
	__set_PSP(tasks[0].stack_pointer);
	__set_CONTROL(0x02);
	task1();
	while(1);
	return 0;
	}
