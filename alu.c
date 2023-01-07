#include "alu.h"

static void update_status_bits(const uint16_t result,
                               const uint8_t a,
                               const uint8_t b,
                               uint8_t* sr);
static uint8_t get_nzvc(const uint16_t result,
                        const uint8_t a,
                        const uint8_t b);

/********************************************************************************
* alu: Returns result after specified aritmetic or logic calculation with 
*      operands a and b. The NZVC bits of referenced status register is 
*      updated in accordance with the result.
*
*      - op_code: OP code, indicates what calculation to perform.
*      - a      : First operand.
*      - b      : Second operand.
*      - sr     : Reference to the status register to be updated.
********************************************************************************/
uint8_t alu(const uint8_t op_code,
            const uint8_t a,
            const uint8_t b,
            uint8_t* sr)
{
   uint16_t result = 0x00;

   if (op_code == ORI || op_code == OR)        result = a | b;
   else if (op_code == ANDI || op_code == AND) result = a & b;
   else if (op_code == XORI || op_code == XOR) result = a ^ b;
   else if (op_code == INC)                    result = a + 1;
   else if (op_code == DEC)                    result = a - 1;
   else if (op_code == ADDI || op_code == ADD) result = a + b;
   else if (op_code == SUBI || op_code == SUB) result = a - b;
   else if (op_code == LSL)                    result = a << 1;
   else if (op_code == LSR)                    result = a >> 1;

   update_status_bits(result, a, b, sr);
   return (uint8_t)result;
}

/********************************************************************************
* alu_compare: Compares specified operands by subtraction and updates the NZVC
*              bits of referenced status register in accordance with the result.
*
*              - a : First operand.
*              - b : Second operand.
*              - sr: Reference to the status register to be updated.
********************************************************************************/
void alu_compare(const uint8_t a,
                 const uint8_t b,
                 uint8_t* sr)
{
   const uint16_t result = a - b;
   update_status_bits(result, a, b, sr);
   return;
}

static inline void update_status_bits(const uint16_t result,
                                     const uint8_t a,
                                     const uint8_t b,
                                     uint8_t* sr)
{
   *sr &= 0xF0;
   *sr |= get_nzvc(result, a, b);
   return;
}

static uint8_t get_nzvc(const uint16_t result,
                        const uint8_t a,
                        const uint8_t b)
{
   uint8_t nzvc = 0x00;

   if (result & (1 << 7)) nzvc |= (1 << N);
   if (result == 0)       nzvc |= (1 << Z);
   if (result & (1 << 9)) nzvc |= (1 << V);

   if (read(result, 7)) set(nzvc, 3);
   if (result == 0)     set(nzvc, 2);
   if (read(result, 9)) set(nzvc, 0);

   if (read(a, 7) != read(b, 7))
   {
      if (read(a, 7) != read(result, 7))
      {
         set(nzvc, C);
      }
   }

   return nzvc;
}
