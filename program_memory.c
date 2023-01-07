#include "program_memory.h"

#define ISR_PCINT0     0x04
#define ISR_PCINT0_end ISR_PCINT0 + 4

#define main ISR_PCINT0 + 5
#define main_loop main + 1
#define setup main + 2
#define led_toggle setup + 10
#define led_toggle_end led_toggle + 4
#define led_on led_toggle + 5
#define led_off led_on + 6
#define end led_off + 6

#define LED1    0
#define BUTTON1 5

#define led_enabled 100

static uint32_t data[PROGRAM_MEMORY_ADDRESS_WIDTH];
static bool program_memory_initialized = false;

static inline uint32_t assemble(const uint8_t op_code,
                                const uint8_t op1,
                                const uint8_t op2);

/********************************************************************************
* program_memory_writes: Writes instructions to program memory. This function
*                        should be called once when the program starts.
********************************************************************************/
void program_memory_write(void)
{
   if (program_memory_initialized) return;

   /********************************************************************************
   * RESET_vect: Reset vector and start address for the program. A jump is made 
   *             to the main subroutine in order to start the program.
   ********************************************************************************/
   data[RESET_vect]     = assemble(JMP, main, 0x00);    /* JMP main */
   data[RESET_vect + 1] = assemble(NOP, 0x00, 0x00);    /* NOP */

   /********************************************************************************
   * PCINT0_vect: Interrupt vector for pin change interrupt at I/O-port B.
   *              Corresponding interrupt routine is executed to handle 
   *              the interrupt.
   ********************************************************************************/
   data[PCINT0_vect]     = assemble(JMP, ISR_PCINT0, 0x00); /* JMP ISR_PCINT0 */
   data[PCINT0_vect + 1] = assemble(NOP, 0x00, 0x00);       /* NOP */

   /********************************************************************************
   * ISR_PCINT0: Interrupt routine for pin change interrupt at I/O-port B.
   *             LED1 is toggled if BUTTON1 is pressed.
   ********************************************************************************/
   data[ISR_PCINT0]     = assemble(IN, R24, PINB);              /* IN R24, PINB */
   data[ISR_PCINT0 + 1] = assemble(ANDI, R24, (1 << BUTTON1));  /* CPI R24, 0x00 */
   data[ISR_PCINT0 + 2] = assemble(BREQ, ISR_PCINT0_end, 0x00); /* BREQ ISR_PCINT0_end */
   data[ISR_PCINT0 + 3] = assemble(CALL, led_toggle, 0x00);     /* CALL led_toggle */
   data[ISR_PCINT0_end] = assemble(RETI, 0x00, 0x00);           /* RETI */

   /********************************************************************************
   * main: Initiates the system at start. A loop is then generated to keep the
   *       program running continuously.
   ********************************************************************************/
   data[main]      = assemble(CALL, setup, 0x00); /* CALL setup */
   data[main_loop] = assemble(JMP, main_loop, 0); /* JMP main_loop */

   /********************************************************************************
   * setup: Initiates I/O-ports (sets LED1 to output, enables the internal pull-up
   *        resistor for BUTTON1) and enables pin change interrupt at BUTTON1. 
   ********************************************************************************/
   data[setup]     = assemble(LDI, R16, (1 << LED1));    /* LDI, R16, (1 << LED1) */
   data[setup + 1] = assemble(OUT, DDRB, R16);           /* OUT DDRB, R16 */
   data[setup + 2] = assemble(LDI, R16, (1 << BUTTON1)); /* LDI R16, (1 << BUTTON1) */
   data[setup + 3] = assemble(OUT, PORTB, R16);          /* OUT PORTB, R16 */
   data[setup + 4] = assemble(SEI, 0x00, 0x00);          /* SEI */
   data[setup + 5] = assemble(LDI, R16, (1 << PCIE0));   /* LDI R16, (1 << PCIE0) */
   data[setup + 6] = assemble(STS, PCICR, R16);          /* STS PCICR, R16 */
   data[setup + 7] = assemble(LDI, R16, (1 << BUTTON1)); /* LDI R16, (1 << BUTTON1) */
   data[setup + 8] = assemble(STS, PCMSK0, R16);         /* STS PCMSK0, R16 */
   data[setup + 9] = assemble(RET, 0x00, 0x00);          /* RET */

   /********************************************************************************
   * led_toggle: Toggles LED1.
   ********************************************************************************/
   data[led_toggle]     = assemble(LDS, R16, led_enabled); /* LDS R16, led_enabled */
   data[led_toggle + 1] = assemble(CPI, R16, 0x00);        /* CPI R16, 0x00 */
   data[led_toggle + 2] = assemble(BREQ, led_on, 0x00);    /* BREQ led_on */
   data[led_toggle + 3] = assemble(JMP, led_off, 0x00);    /* JMP led_off */
   data[led_toggle + 4] = assemble(RET, 0x00, 0x00);       /* RET */

   /********************************************************************************
   * led_on: Enables LED1 and stores current state in data memory.
   ********************************************************************************/
   data[led_on]     = assemble(IN, R16, PORTB);            /* IN R16, PORTB */
   data[led_on + 1] = assemble(ORI, R16, (1 << LED1));     /* ORI R16, (1 << LED1) */
   data[led_on + 2] = assemble(OUT, PORTB, R16);           /* OUT PORTB, R16 */
   data[led_on + 3] = assemble(LDI, R16, 0x01);            /* LDI R16, 0x01 */
   data[led_on + 4] = assemble(STS, led_enabled, R16);     /* STS led_enabled, R16 */
   data[led_on + 5] = assemble(JMP, led_toggle_end, 0x00); /* JMP led_toggle_end */

   /********************************************************************************
   * led_off: Disables LED1 and stores current state in data memory.
   ********************************************************************************/
   data[led_off]     = assemble(IN, R16, PORTB);            /* IN R16, PORTB */
   data[led_off + 1] = assemble(ANDI, R16, ~(1 << LED1));   /* ANDI R16, ~(1 << LED1) */
   data[led_off + 2] = assemble(OUT, PORTB, R16);           /* OUT PORTB, R16 */
   data[led_off + 3] = assemble(LDI, R16, 0x00);            /* LDI R16, 0x01 */
   data[led_off + 4] = assemble(STS, led_enabled, R16);     /* STS led_enabled, R16 */
   data[led_off + 5] = assemble(JMP, led_toggle_end, 0x00); /* JMP led_toggle_end */

   program_memory_initialized = true;
   return;
}

/********************************************************************************
* program_memory_read: Returns the instruction at specified address. If an
*                      invalid address is specified (should be impossible as
*                      long as the program memory address width isn't increased)
*                      no operation (0x00) is returned.
*
*                      - address: Address to instruction in program memory.
********************************************************************************/
uint32_t program_memory_read(const uint8_t address)
{
   if (address < PROGRAM_MEMORY_ADDRESS_WIDTH)
   {
      return data[address];
   }
   else
   {
      return 0x00;
   }
}

/********************************************************************************
* program_memory_subroutine_name: Returns the name of the subroutine at
*                                 specified address.
*
*                                 - address: Address within the subroutine.
********************************************************************************/
const char* program_memory_subroutine_name(const uint8_t address)
{
   if (address >= RESET_vect && address < PCINT0_vect) return "RESET_vect";
   else if (address >= PCINT0_vect && address < ISR_PCINT0) return "PCINT0_vect";
   else if (address >= ISR_PCINT0 && address < main) return "ISR_PCINT0";
   else if (address >= main && address < setup) return "main";
   else if (address >= setup && address < led_toggle) return "setup";
   else if (address >= led_toggle && address < led_on) return "led_toggle";
   else if (address >= led_on && address < led_off) return "led_on";
   else if (address >= led_off && address < end) return "led_off";
   else return "Unknown";
}

static inline uint32_t assemble(const uint8_t op_code,
                                const uint8_t op1,
                                const uint8_t op2)
{
   uint32_t instruction = op_code << 16;
   instruction |= op1 << 8;
   instruction |= op2;
   return instruction;
}
