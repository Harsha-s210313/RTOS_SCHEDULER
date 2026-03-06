# Bare Metal Round-Robin Scheduler — STM32F411RE

A preemptive round-robin task scheduler built from scratch on the STM32F411RE microcontroller, implementing context switching, PSP-based task isolation, and SysTick-driven preemption — without any RTOS library.

---

## 1. Overview

This project implements a minimal preemptive scheduler entirely from scratch on bare metal ARM Cortex-M4. The scheduler supports up to 4 concurrent tasks, switches between them every 1ms using the SysTick timer, and performs full CPU context save and restore using the PendSV exception.

The goal is to understand exactly what an RTOS does under the hood — before moving to FreeRTOS. Every concept here — TCBs, stack initialization, context switching, PendSV — maps directly to how production RTOS kernels work internally.

---

## 2. How It Works

### Boot Sequence
```
main()
  │
  ├── task_create(task1..4)   → initialize TCBs and fake stack frames
  ├── systick_init(1000)      → configure 1ms SysTick interrupt
  ├── __set_PSP(...)          → point PSP to Task1's stack
  ├── __set_CONTROL(0x02)     → switch CPU to use PSP instead of MSP
  └── task1()                 → start executing Task1
```

### Every 1ms (SysTick fires)
```
SysTick_Handler()
  │
  ├── current_task++          → select next task (wrap at task_count)
  └── trigger PendSV          → SCB->ICSR |= PENDSVSET
```

### Context Switch (PendSV fires at lowest priority)
```
PendSV_Handler()
  │
  ├── MRS   → read current PSP
  ├── STMDB → push R4-R11 onto current task's stack
  ├── save PSP → tasks[prev].stack_pointer
  ├── load PSP → tasks[current].stack_pointer
  ├── LDMIA → pop R4-R11 from next task's stack
  └── MSR   → write new PSP → CPU resumes next task
```

---

## 3. Key Concepts Implemented

| Concept | Implementation |
|---|---|
| Task Control Block (TCB) | `TCB_t` struct with stack, stack pointer, state, function pointer |
| Stack Isolation | Each task has its own private `uint32_t stack[256]` |
| Fake Context Frame | Stack pre-filled with xPSR, PC, LR, R0-R3, R12 at task creation |
| Preemption | SysTick fires every 1ms and triggers PendSV |
| Context Save | R4-R11 saved manually via `STMDB` in PendSV handler |
| Context Restore | R4-R11 restored via `LDMIA`, hardware restores R0-R3, R12, LR, PC, xPSR |
| PSP/MSP Split | Kernel uses MSP, tasks use PSP — clean separation |
| Round Robin | Tasks cycle in fixed order: Task1 → Task2 → Task3 → Task4 → Task1 |

---

## 4. Project Structure

```
RTOS_SCHEDULER/
├── Inc/
│   ├── scheduler.h      # TCB struct, TaskState enum, STACK_SIZE, MAX_TASKS
│   ├── task.h           # task_create(), task_get_current() declarations
│   └── systick.h        # systick_init(), SysTick_Handler() declarations
├── Src/
│   ├── main.c           # Task definitions, main() entry point
│   ├── scheduler.c      # PendSV_Handler — full context switch in assembly
│   ├── task.c           # task_create(), stack initialization, TCB array
│   └── systick.c        # systick_init(), SysTick_Handler()
├── Startup/
│   └── startup_stm32f411retx.s
├── STM32F411RETX_FLASH.ld
└── STM32F411RETX_RAM.ld
```

---

## 5. Hardware Requirements

| Component | Details |
|---|---|
| MCU Board | STM32F411RE Nucleo |
| LED | On-board PA5 (Task 1 — heartbeat) |
| UART | PA2 TX → USB-Serial adapter (Task 2 — logging) |
| Button | PC13 on-board user button (Task 4 — input) |
| Programmer | ST-LINK V2 (on-board) |

---

## 6. Software Requirements

- STM32CubeIDE v1.x or later
- ARM GCC Toolchain (arm-none-eabi-gcc)
- CMSIS headers for STM32F4xx

---

## 7. Task Descriptions

### Task 1 — LED Heartbeat
Toggles the on-board LED on PA5 every ~600ms. Acts as a visual system health indicator — if the LED stops blinking the scheduler has stalled.

### Task 2 — UART Logger
Initializes USART2 at 9600 baud and repeatedly transmits a status string over PA2. Demonstrates concurrent communication alongside other tasks.

### Task 3 — Compute Task
Runs a continuous arithmetic workload — addition and XOR operations on a volatile variable. Simulates a CPU-intensive background task to verify the scheduler preempts correctly under load.

### Task 4 — Button Monitor
Monitors the on-board user button on PC13. Toggles PA5 LED on button press with software debouncing. Demonstrates input handling as a concurrent task.

---

## 8. Memory Layout

Each task gets its own private stack of 256 words (1KB) in SRAM:

```
SRAM (128KB starting at 0x20000000)
┌─────────────────┐
│   Task 1 Stack  │  256 words (1KB)
├─────────────────┤
│   Task 2 Stack  │  256 words (1KB)
├─────────────────┤
│   Task 3 Stack  │  256 words (1KB)
├─────────────────┤
│   Task 4 Stack  │  256 words (1KB)
├─────────────────┤
│   Global Data   │
├─────────────────┤
│   MSP Stack     │  Kernel/interrupt stack
└─────────────────┘
```

---

## 9. Stack Initialization

When a task is created its stack is pre-filled to look like it was already running and got preempted by an interrupt. The ARM Cortex-M4 hardware expects this layout on the PSP:

```
High Address
┌──────────┐
│  xPSR    │  0x01000000 — Thumb bit set
├──────────┤
│  PC      │  Task function address
├──────────┤
│  LR      │  0x00000000
├──────────┤
│  R12     │  0x00000000
├──────────┤
│  R3      │  0x00000000
├──────────┤
│  R2      │  0x00000000
├──────────┤
│  R1      │  0x00000000
├──────────┤
│  R0      │  0x00000000
└──────────┘
Low Address  ← Initial PSP points here
```

When PendSV restores this task for the first time, the CPU pops PC and jumps directly to the task function.

---

## 10. Context Switch — Assembly Detail

The PendSV handler performs the full context switch:

```asm
; Save current task context
MRS   R0, PSP              ; Get current Process Stack Pointer
STMDB R0!, {R4-R11}        ; Push callee-saved registers onto task stack

; Save updated PSP into TCB
STR   R0, [TCB_prev]       ; tasks[prev_task].stack_pointer = PSP

; Load next task context
LDR   R0, [TCB_next]       ; PSP = tasks[current_task].stack_pointer
LDMIA R0!, {R4-R11}        ; Pop callee-saved registers from next task stack

; Restore PSP
MSR   PSP, R0              ; Set PSP to next task's stack
BX    LR                   ; Return — hardware pops R0-R3, R12, LR, PC, xPSR
```

Hardware automatically saves and restores R0-R3, R12, LR, PC, and xPSR on exception entry and exit.

---

## 11. Build Instructions

1. Clone the repository
2. Open STM32CubeIDE and import the project
3. Add include path: `Inc/`
4. Build → Project → Build Project (Ctrl+B)

**Expected output:**
```
Build Finished. 0 errors, 0 warnings.
```

---

## 12. Key Learnings

- ARM Cortex-M4 has two stack pointers — MSP for kernel/interrupts, PSP for tasks
- PendSV is the dedicated context switch exception — always runs at lowest priority
- A context switch saves R4-R11 manually; hardware saves R0-R3, R12, LR, PC, xPSR automatically
- Task stacks must be pre-initialized with a valid fake context frame
- Round-robin scheduling gives equal CPU time to all tasks with no priority
- This is exactly what FreeRTOS does internally — just with priorities, queues, and semaphores added on top

---

## 13. Next Steps

- Add task priorities — higher priority tasks preempt lower ones
- Implement `task_delay()` — block a task for N milliseconds
- Add semaphores for shared resource protection
- Migrate to FreeRTOS and compare the internals
- Add a fifth idle task that runs when all others are blocked

---

## 14. Build Output

```
12:57:11 **** Build of configuration Debug for project RTOS_SCHEDULER ****
Build Finished. 0 errors, 0 warnings.
```

---

*Built as part of an embedded systems learning series. Hardware testing pending STM32F411RE Nucleo board arrival.*
