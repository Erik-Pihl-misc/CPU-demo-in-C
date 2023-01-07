#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define NOP  0x00
#define LDI  0x01
#define MOV  0x02
#define OUT  0x03
#define IN   0x04
#define STS  0x05
#define LDS  0x06
#define CLR  0x07
#define ORI  0x08
#define ANDI 0x09
#define XORI 0x0A
#define OR   0x0B
#define AND  0x0C
#define XOR  0x0D
#define ADDI 0x0E
#define SUBI 0x0F

#define ADD  0x10
#define SUB  0x11
#define INC  0x12
#define DEC  0x13
#define CPI  0x14
#define CP   0x15
#define JMP  0x16
#define BREQ 0x17
#define BRNE 0x18
#define BRGE 0x19
#define BRGT 0x1A
#define BRLE 0x1B
#define BRLT 0x1C
#define CALL 0x1D
#define RET  0x1E
#define RETI 0x1F

#define PUSH 0x20
#define POP  0x21
#define LSL  0x22
#define LSR  0x23
#define SEI  0x24
#define CLI  0x25

#define DDRB  0x00
#define PORTB 0x01
#define PINB  0x02

#define DDRC  0x03
#define PORTC 0x04
#define PINC  0x05

#define DDRD  0x06
#define PORTD 0x07
#define PIND  0x08

#define PCICR 0x09
#define PCIFR 0x0A

#define PCMSK0 0x10
#define PCMSK1 0x11
#define PCMSK2 0x12

#define PCIE0 0
#define PCIE1 1
#define PCIE2 2

#define PCIF0 0
#define PCIF1 1
#define PCIF2 2

#define RESET_vect  0x00
#define PCINT0_vect 0x02
#define PCINT1_vect 0x04
#define PCINT2_vect 0x06

#define R0  0x00
#define R1  0x01
#define R2  0x02
#define R3  0x03
#define R4  0x04
#define R5  0x05
#define R6  0x06
#define R7  0x07
#define R8  0x08
#define R9  0x09
#define R10 0x0A
#define R11 0x0B
#define R12 0x0C
#define R13 0x0D
#define R14 0x0E
#define R15 0x0F

#define R16 0x10
#define R17 0x11
#define R18 0x12
#define R19 0x13
#define R20 0x14
#define R21 0x15
#define R22 0x16
#define R23 0x17
#define R24 0x18
#define R25 0x19
#define R26 0x1A
#define R27 0x1B
#define R28 0x1C
#define R29 0x1D
#define R30 0x1E
#define R31 0x1F

#define CPU_REGISTER_ADDRESS_WIDTH 32
#define CPU_REGISTER_DATA_WIDTH    8
#define IO_REGISTER_DATA_WIDTH     8

#define I 4
#define N 3
#define Z 2
#define V 1
#define C 0

#define set(reg, bit)  reg |= (1 << bit)
#define clr(reg, bit)  reg &= ~(1 << (bit))
#define read(reg, bit) (reg & (1 << (bit)))

enum cpu_state
{
   CPU_STATE_FETCH,
   CPU_STATE_DECODE,
   CPU_STATE_EXECUTE
};

/********************************************************************************
* cpu_instruction_name: Returns the name of specified instruction.
* 
*                       - instruction: The specified CPU instruction.
********************************************************************************/
const char* cpu_instruction_name(const uint8_t instruction);

/********************************************************************************
* cpu_state_name: Returns the name of specified CPU state.
*
*                 - state: The specified CPU state.
********************************************************************************/
const char* cpu_state_name(const enum cpu_state state);

/********************************************************************************
* cpu_register_name: Returns the name of specified CPU register.
*
*                    - reg: The specified CPU register.
********************************************************************************/
static const char* cpu_register_name(const uint8_t reg)
{
   if (reg < CPU_REGISTER_ADDRESS_WIDTH)
   {
      static char s[10] = { '\0' };
      sprintf(s, "R%hu", reg);
      return s;
   }
   else
   {
      return "Unknown";
   }
}

/********************************************************************************
* get_binary: Returns specified number as a binary string with specified 
*             minimum number of characters.
*
*             - num      : The specified number.
*             - min_chars: Minimum number of characters in the returned string.
********************************************************************************/
const char* get_binary(uint32_t num, 
                       const uint8_t min_chars);

#endif /* CPU_H_ */