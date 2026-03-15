# RTOS Scheduler — STM32F411RE

A preemptive task scheduler implemented **twice** on the STM32F411RE (ARM Cortex-M4):
- `bare-metal/` — built entirely from scratch, no RTOS library
- `freertos/` — built on FreeRTOS, no HAL, no CubeMX code generation

The goal: understand what an RTOS does internally, then validate that understanding by implementing the same concepts using FreeRTOS.

---

## Repository Structure
```
rtos-scheduler/
├── bare-metal/         # Custom scheduler — TCB, SysTick, PendSV assembly
└── freertos/           # FreeRTOS scheduler — queues, semaphores, mutex, ISR
```

---

## bare-metal — Custom Scheduler

Built from scratch on ARM Cortex-M4 with no RTOS library.

### How it works

| Mechanism | Implementation |
|---|---|
| Task Control Block | `TCB_t` struct — stack pointer, state, function pointer |
| Stack isolation | Each task has a private `uint32_t stack[256]` (1KB) |
| Preemption | SysTick fires every 1ms, triggers PendSV |
| Context save | `STMDB` pushes R4–R11 onto task stack in PendSV handler |
| Context restore | `LDMIA` pops R4–R11; hardware restores R0–R3, R12, LR, PC, xPSR |
| PSP/MSP split | Kernel uses MSP, tasks use PSP |
| Scheduling | Round-robin — Task1 → Task2 → Task3 → Task4 → repeat |

### Context switch (PendSV handler)
```asm
MRS   R0, PSP              ; read current PSP
STMDB R0!, {R4-R11}        ; push callee-saved regs onto task stack
STR   R0, [TCB_prev]       ; save PSP into current TCB
LDR   R0, [TCB_next]       ; load PSP from next TCB
LDMIA R0!, {R4-R11}        ; pop callee-saved regs from next task stack
MSR   PSP, R0              ; write new PSP
BX    LR                   ; return — hardware restores R0-R3, R12, LR, PC, xPSR
```

### Tasks

| Task | Function |
|---|---|
| Task 1 | LED heartbeat — toggles PA5 every ~600ms |
| Task 2 | UART logger — transmits status over USART2 at 9600 baud |
| Task 3 | Compute task — arithmetic workload to verify preemption under load |
| Task 4 | Button monitor — PC13 input with software debouncing |

---

## freertos — FreeRTOS Scheduler

Built on FreeRTOS with no STM32CubeMX code generation and no HAL abstraction.

### FreeRTOS concepts demonstrated

| Concept | Implementation |
|---|---|
| Queue | `vTask1` produces, `vTask2` consumes — 5-item int queue |
| Binary semaphore + ISR | Button press → `xSemaphoreGiveFromISR()` → `vTask4` unblocks |
| Mutex | `xUARTmutex` prevents garbled output from `vTask2` + `vTask3` |
| Priority preemption | `vTask3` (priority 2) preempts `vTask1/2` (priority 1) every 100ms |

### Task priorities

| Task | Priority | Role |
|---|---|---|
| `vTask1` | 1 | Queue producer — sends counter every 1s |
| `vTask2` | 1 | Queue consumer — prints via UART |
| `vTask3` | 2 | Periodic UART message every 100ms |
| `vTask4` | 3 | Button handler — ISR semaphore → LED toggle |

### Configuration

| Parameter | Value |
|---|---|
| Port | GCC / ARM_CM4F |
| Heap | `heap_4` (8KB) |
| Tick rate | 1000 Hz (1ms) |
| Max priorities | 5 |
| CPU clock | 100 MHz |

---

## Hardware

| Component | Detail |
|---|---|
| MCU | STM32F411RE (Cortex-M4F @ 100MHz) |
| Board | Nucleo-F411RE |
| LED | PA5 (onboard) |
| Button | PC13 (onboard USER button) |
| UART | USART2 via PA2 (ST-Link Virtual COM Port) |

---

## Build

1. Open STM32CubeIDE
2. Import the `bare-metal/` or `freertos/` folder as an existing project
3. Press `Ctrl+B`

Expected: `Build Finished. 0 errors, 0 warnings.`

---

## Key takeaways

- ARM Cortex-M4 has two stack pointers — MSP for kernel/interrupts, PSP for tasks
- PendSV is the dedicated context switch exception — always lowest priority
- Context switch manually saves R4–R11; hardware handles R0–R3, R12, LR, PC, xPSR
- FreeRTOS queues, semaphores, and mutexes map directly to the primitives built in the bare-metal version
- Priority inheritance in mutexes prevents priority inversion — not available in bare-metal version
