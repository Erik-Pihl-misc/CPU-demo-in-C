#include "stack.h"

static uint8_t data[STACK_ADDRESS_WIDTH];
static uint8_t sp = STACK_ADDRESS_WIDTH - 1;
static bool stack_empty = true;

/********************************************************************************
* stack_reset: Clears content of the stack.
********************************************************************************/
void stack_reset(void)
{
   for (uint8_t* i = data; i < data + STACK_ADDRESS_WIDTH; ++i)
   {
      *i = 0x00;
   }

   sp = STACK_ADDRESS_WIDTH - 1;
   stack_empty = true;
   return;
}

/********************************************************************************
* stack_push: Pushes 8-bit value to the stack and returns 0 after successful
*             execution. If the stack is full, the value isn't pushed to the
*             stack and error code 1 is returned.
*
*             - value: The value to push to the stack.
********************************************************************************/
int stack_push(const uint8_t value)
{
   if (sp > 0)
   {
      if (stack_empty)
      {
         data[sp] = value;
         stack_empty = false;
      }
      else
      {
         data[--sp] = value;
      }
      return 0;
   }
   else
   {
      return 1;
   }
}

/********************************************************************************
* stack_pop: Pops 8-bit value from the stack, stores at designated address and
*            returns 0 after successful execution. If the stack is empty, no
*            value is stored at designated address and error code 1 is returned.
*
*            - destination: Address to store the value popped from the stack.
********************************************************************************/
int stack_pop(uint8_t* destination)
{
   if (stack_empty)
   {
      return 1;
   }
   else
   {
      *destination = data[sp];

      if (sp < STACK_ADDRESS_WIDTH - 1)
      {
         sp++;
      }
      else
      {
         stack_empty = true;
      }
      return 0;
   }
}