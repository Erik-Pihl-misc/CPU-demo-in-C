#ifndef PROGRAM_MEMORY_H_
#define PROGRAM_MEMORY_

#include "cpu.h"

#define PROGRAM_MEMORY_ADDRESS_WIDTH 256
#define PROGRAM_MEMORY_DATA_WIDTH    32

/********************************************************************************
* program_memory_writes: Writes instructions to program memory. This function
*                        should be called once when the program starts.
********************************************************************************/
void program_memory_write(void);

/********************************************************************************
* program_memory_read: Returns the instruction at specified address. If an
*                      invalid address is specified (should be impossible as
*                      long as the program memory address width isn't increased)
*                      no operation (0x00) is returned.
* 
*                      - address: Address to instruction in program memory.
********************************************************************************/
uint32_t program_memory_read(const uint8_t address);

/********************************************************************************
* program_memory_subroutine_name: Returns the name of the subroutine at
*                                 specified address.
* 
*                                 - address: Address within the subroutine.
********************************************************************************/
const char* program_memory_subroutine_name(const uint8_t address);

#endif /* PROGRAM_MEMORY_ */