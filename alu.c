#include "alu.h"

static void update_status_bits(const uint16_t result,
                               const uint8_t op_code,
                               const uint8_t a,
                               const uint8_t b,
                               uint8_t* sr);
                                                            
static uint8_t get_nzvc(const uint16_t result,
                        const uint8_t op_code,
                        const uint8_t a,
                        const uint8_t b);

static inline bool overflow_occured(const uint16_t result,
                                    const uint8_t op_code, 
                                    const uint8_t a, 
                                    const uint8_t b);
                                    
static inline void check_for_two_complement(uint16_t* result);                                  
static inline bool addition_performed(const uint8_t op_code);
static inline bool subtraction_performed(const uint8_t op_code);

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

   check_for_two_complement(&result);
   update_status_bits(result, op_code, a, b, sr);
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
   uint16_t result;
   result = a - b;
   check_for_two_complement(&result);
   update_status_bits(result, SUB, a, b, sr);
   return;
}

static inline void update_status_bits(const uint16_t result,
                                     const uint8_t op_code,
                                     const uint8_t a,
                                     const uint8_t b,
                                     uint8_t* sr)
{
   *sr &= 0xF0;

   if (op_code == INC || op_code == DEC)
   {
      *sr |= get_nzvc(result, op_code, a, 1);
   }
   else 
   {
      *sr |= get_nzvc(result, op_code, a, b);
   }
   return;
}

static uint8_t get_nzvc(const uint16_t result,
                        const uint8_t op_code,
                        const uint8_t a,
                        const uint8_t b)
{
   uint8_t nzvc = 0x00;

   if (read(result, 7))
   {
      set(nzvc, N);
   }

   if (result == 0)
   {
      set(nzvc, Z);
   }

   if (read(result, 9))
   {
      set(nzvc, C);
   }

   if (overflow_occured(result, op_code, a, b))
   {
      set(nzvc, V);
   }

   return nzvc;
}

static inline bool overflow_occured(const uint16_t result,
                                    const uint8_t op_code,
                                    const uint8_t a,
                                    const uint8_t b)
{
   if (addition_performed(op_code) && read(a, 7) == read(b, 7))
   {
      if (read(a, 7) != read(result, 7))
      {
         return true;
      }
   }
   else if (subtraction_performed(op_code) && read(a, 7) != read(b, 7))
   {
      if (read(b, 7) == read(result, 7))
      {
         return true;
      }
   }

   return false;
}

static inline void check_for_two_complement(uint16_t* result)
{
   if (*result > 0xFF) 
   {
      *result += 0xFF;
   }
   return;
}

static inline bool addition_performed(const uint8_t op_code)
{
   if (op_code == ADD || op_code == ADDI || op_code == INC)
   {
      return true;
   }
   else
   {
      return false;
   }
}

static inline bool subtraction_performed(const uint8_t op_code)
{
   if (op_code == SUB || op_code == SUBI || op_code == DEC)
   {
      return true;
   }
   else
   {
      return false;
   }
}
