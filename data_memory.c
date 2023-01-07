#include "data_memory.h"

static uint8_t data[DATA_MEMORY_ADDRESS_WIDTH];

/********************************************************************************
* data_memory_reset: Clears content of the data memory.
********************************************************************************/
void data_memory_reset(void)
{
   for (uint8_t* i = data; i < data + DATA_MEMORY_ADDRESS_WIDTH; ++i)
   {
      *i = 0x00;
   }
   return;
}

/********************************************************************************
* data_memory_write: Writes 8-bit value to specified address in data memory.
*
*                    - address: Data memory address to write to.
*                    - value  : Data to write to specified address.
********************************************************************************/
int data_memory_write(const uint16_t address, 
                      const uint8_t value)
{
   if (address < DATA_MEMORY_ADDRESS_WIDTH)
   {
      data[address] = value;
      return 0;
   }
   else
   {
      return 1;
   }
}

/********************************************************************************
* data_memory_read: Reads 8-bit value from specified address in data memory.
*                   If an invalid address is specified, 0x00 is returned.
*
*                   - address: Data memory address to read from.
********************************************************************************/
uint8_t data_memory_read(const uint16_t address)
{
   if (address < DATA_MEMORY_ADDRESS_WIDTH)
   {
      return data[address];
   }
   else
   {
      return 0;
   }
}