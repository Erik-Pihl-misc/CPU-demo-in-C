#ifndef CPU_CONTROLLER_H_
#define CPU_CONTROLLER_H_

#include "cpu.h"
#include "control_unit.h"

/********************************************************************************
* cpu_controller_run_by_input: Controls the program flow and input to the PINB
*                              register by input from the keyboard.
********************************************************************************/
void cpu_controller_run_by_input(void);

#endif /* CPU_CONTROLLER_H_ */