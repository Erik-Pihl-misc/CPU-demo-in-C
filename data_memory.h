#ifndef DATA_MEMORY_H_
#define DATA_MEMORY_H_

#include "cpu.h"

#define DATA_MEMORY_ADDRESS_WIDTH 2000
#define DATA_MEMORY_DATA_WIDTH    8

/********************************************************************************
* data_memory_reset: Clears content of the data memory.
********************************************************************************/
void data_memory_reset(void);

/********************************************************************************
* data_memory_write: Writes 8-bit value to specified address in data memory.
* 
*                    - address: Data memory address to write to.
*                    - value  : Data to write to specified address.
********************************************************************************/
int data_memory_write(const uint16_t address, const uint8_t value);

/********************************************************************************
* data_memory_read: Reads 8-bit value from specified address in data memory.
*                   If an invalid address is specified, 0x00 is returned.
*
*                   - address: Data memory address to read from.
********************************************************************************/
uint8_t data_memory_read(const uint16_t address);

#endif /* DATA_MEMORY_H_ */