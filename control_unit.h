#ifndef CONTROL_UNIT_H_
#define CONTROL_UNIT_H_

#include "cpu.h"
#include "program_memory.h"
#include "data_memory.h"
#include "stack.h"
#include "alu.h"

/********************************************************************************
* control_unit_reset: Resets control unit and corresponding program.
********************************************************************************/
void control_unit_reset(void);

/********************************************************************************
* control_unit_run_next_state: Runs next state in the CPU instruction cycle.
********************************************************************************/
void control_unit_run_next_state(void);

/********************************************************************************
* control_unit_run_next_state: Runs next CPU instruction cycle, i.e. fetches
*                              a new instruction from program memory, decodes
*                              and executes it.
********************************************************************************/
void control_unit_run_next_instruction_cycle(void);

/********************************************************************************
* control_unit_print: Prints information about the processor, for instance
*                     current subroutine, instruction, state, content in
*                     CPU-registers and I/O registers DDRB, PORTB and PINB.
********************************************************************************/
void control_unit_print(void);

/********************************************************************************
* control_unit_interrupt_enabled: Indicates if interrupt is enabled by check
*                                 the I flag in the status register.
********************************************************************************/
extern bool (*control_unit_interrupt_enabled)(void);

/********************************************************************************
* control_unit_generate_interrupt: Generates interrupt by storing content of
*                                  miscellaneous registers, current state,
*                                  program counter etc and jumps to specified
*                                  interrupt vector. Corresponding flag bit
*                                  is stored for clearing it after the interrupt
*                                  is handled.
*  
*                                  - interrupt_vector: Address in program memory
*                                                      that will be assigned to
*                                                      the program counter.
*                                  - flag_bit        : Set flag bit that caused 
*                                                      the interrupt.
********************************************************************************/
extern bool (*control_unit_enable_interrupt)(const uint8_t interrupt_vector,
                                             const uint8_t flag_bit);

#endif /* CONTROL_UNIT_H_ */