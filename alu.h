#ifndef ALU_H_
#define ALU_H_

#include "cpu.h"

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
            uint8_t* sr);

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
                 uint8_t* sr);

#endif /* ALU_H_ */

