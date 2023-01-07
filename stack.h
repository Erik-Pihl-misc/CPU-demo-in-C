#ifndef STACK_H_
#define STACK_H_

#include "cpu.h"

#define STACK_ADDRESS_WIDTH 256
#define STACK_DATA_WIDTH    8

/********************************************************************************
* stack_reset: Clears content of the stack.
********************************************************************************/
void stack_reset(void);

/********************************************************************************
* stack_push: Pushes 8-bit value to the stack and returns 0 after successful
*             execution. If the stack is full, the value isn't pushed to the 
*             stack and error code 1 is returned. 
* 
*             - value: The value to push to the stack.
********************************************************************************/
int stack_push(const uint8_t value);

/********************************************************************************
* stack_pop: Pops 8-bit value from the stack, stores at designated address and 
*            returns 0 after successful execution. If the stack is empty, no
*            value is stored at designated address and error code 1 is returned.
* 
*            - destination: Address to store the value popped from the stack.
********************************************************************************/
int stack_pop(uint8_t* destination);

#endif /* STACK_H_ */