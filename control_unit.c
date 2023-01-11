#include "control_unit.h"
#include "pci_regs.h"

static inline void cpu_registers_reset(void);
static inline bool equal(void);
static inline bool greater(void);
static inline bool lower(void);

static inline bool interrupt_enabled(void);
static inline void monitor_interrupts(void);
static void generate_interrupt(const uint8_t interrupt_vector,
                               const uint8_t flag_bit);
static void return_from_interrupt(void);

/* Static variables: */
static uint32_t ir;    /* Instruction register, stores next instruction to execute. */
static uint8_t pc;     /* Program counter, stores address to next instruction to fetch. */
static uint8_t mar;    /* Memory address register, stores address for current instruction. */
static uint8_t sr;     /* Status register, stores status bits INZVC. */

static uint8_t op_code; /* Stores OP-code, for example LDI, OUT, JMP etc. */
static uint8_t op1;     /* Stores first operand, most often a destination. */
static uint8_t op2;     /* Stores second operand, most often a value or read address. */

static uint8_t reg[CPU_REGISTER_ADDRESS_WIDTH]; /* CPU-registers R0 - R31. */
static enum cpu_state state;                    /* Stores current state. */

static uint8_t interrupt_source;                /* Vector för interrupt source. */

static struct pci_regs_vtable pci_regs_vtable =
{
   .interrupt_enabled = interrupt_enabled,
   .generate_interrupt = generate_interrupt
};

static struct pci_regs pci_regs_b =
{
   .pin_reg = PINB,
   .mask_reg = PCMSK0,
   .flag_bit = PCIF0,
   .interrupt_vector = PCINT0_vect,
   .last_value = 0x00,
   .vptr = &pci_regs_vtable
};

static struct pci_regs pci_regs_c =
{
   .pin_reg = PINC,
   .mask_reg = PCMSK1,
   .flag_bit = PCIF1,
   .interrupt_vector = PCINT1_vect,
   .last_value = 0x00,
   .vptr = &pci_regs_vtable
};

static struct pci_regs pci_regs_d =
{
   .pin_reg = PIND,
   .mask_reg = PCMSK2,
   .flag_bit = PCIF2,
   .interrupt_vector = PCINT2_vect,
   .last_value = 0x00,
   .vptr = &pci_regs_vtable
};

/********************************************************************************
* control_unit_reset: Resets control unit and corresponding program.
********************************************************************************/
void control_unit_reset(void)
{
   ir = 0x00;
   pc = 0x00;
   mar = 0x00;
   sr = 0x00;

   op_code = 0x00;
   op1 = 0x00;
   op2 = 0x00;
   state = CPU_STATE_FETCH;
   interrupt_source = RESET_vect;

   pci_regs_b.last_value = 0x00;
   pci_regs_c.last_value = 0x00;
   pci_regs_d.last_value = 0x00;

   cpu_registers_reset();
   program_memory_write();
   data_memory_reset();
   stack_reset();
   return;
}

/********************************************************************************
* control_unit_run_next_state: Runs next state in the CPU instruction cycle.
********************************************************************************/
void control_unit_run_next_state(void)
{
   switch (state)
   {
      case CPU_STATE_FETCH:
      {
         ir = program_memory_read(pc); /* Fetches next instruction. */
         mar = pc;                     /* Stores address of current instruction. */
         pc++;                         /* Program counter points to next instruction. */
         state = CPU_STATE_DECODE;     /* Decodes the instruction during next clock cycle. */
         break;
      }
      case CPU_STATE_DECODE:
      {
         op_code = ir >> 16;           /* Bit 23 downto 16 consist of the OP code. */
         op1 = ir >> 8;                /* Bit 15 downto 8 consists of the first operand. */
         op2 = ir;                     /* Bit 7 downto 0 constist of the second operand. */
         state = CPU_STATE_EXECUTE;    /* Executes the instruction during next clock cycle. */
         break;
      }
      case CPU_STATE_EXECUTE:
      {
         switch (op_code)              /* Executes specified operation. */
         {
            case NOP:
            {
               break;
            }
            case LDI:
            {
               reg[op1] = op2;
               break;
            }
            case MOV:
            {
               reg[op1] = reg[op2];
               break;
            }
            case OUT:
            {
               data_memory_write(op1, reg[op2]);
               break;
            }
            case IN: 
            {
               reg[op1] = data_memory_read(op2);
               break;
            }
            case STS:
            {
               data_memory_write(op1, reg[op2]);

               if (op2 < DATA_MEMORY_DATA_WIDTH - 1)
               {
                  data_memory_write(op1 + 1, reg[op2 + 1]);
               }
               break;
            }
            case LDS:
            {
               reg[op1] = data_memory_read(op2);
               
               if (op1 < CPU_REGISTER_ADDRESS_WIDTH - 1)
               {
                  reg[op1 + 1] = data_memory_read(op2 + 1);
               }

               break;
            }
            case CLR:
            {
               reg[op1] = 0x00;
               break;
            }
            case ORI:
            {
               reg[op1] = alu(op_code, reg[op1], op2, &sr);
               break;
            }
            case ANDI:
            {
               reg[op1] = alu(op_code, reg[op1], op2, &sr);
               break;
            }
            case XORI:
            {
               reg[op1] = alu(op_code, reg[op1], op2, &sr);
               break;
            }
            case OR:
            {
               reg[op1] = alu(op_code, reg[op1], reg[op2], &sr);
               break;
            }
            case AND:
            {
               reg[op1] = alu(op_code, reg[op1], reg[op2], &sr);
               break;
            }
            case XOR:
            {
               reg[op1] = alu(op_code, reg[op1], reg[op2], &sr);
               break;
            }
            case ADDI:
            {
               reg[op1] = alu(op_code, reg[op1], op2, &sr);
               break;
            }
            case SUBI:
            {
               reg[op1] = alu(op_code, reg[op1], op2, &sr);
               break;
            }
            case ADD:
            {
               reg[op1] = alu(op_code, reg[op1], reg[op2], &sr);
               break;
            }
            case SUB:
            {
               reg[op1] = alu(op_code, reg[op1], reg[op2], &sr);
               break;
            }
            case INC:
            {
               reg[op1] = alu(op_code, reg[op1], 0x00, &sr);
               break;
            }
            case DEC:
            {
               reg[op1] = alu(op_code, reg[op1], 0x00, &sr);
               break;
            }
            case LSL:
            {
               reg[op1] = alu(op_code, reg[op1], 0x00, &sr);
               break;
            }
            case LSR:
            {
               reg[op1] = alu(op_code, reg[op1], 0x00, &sr);
               break;
            }
            case CPI:
            {
               alu_compare(reg[op1], op2, &sr);
               break;
            }
            case CP:
            {
               alu_compare(reg[op1], reg[op2], &sr);
               break;
            }
            case JMP:
            {
               pc = op1;
               break;
            }
            case BREQ:
            {
               if (equal()) 
               {
                  pc = op1;
               }
               break;
            }
            case BRNE:
            {
               if (!equal())
               {
                  pc = op1;
               }
               break;
            }
            case BRGE:
            {
               if (greater() || (equal()))
               {
                  pc = op1;
               }
               break;
            }
            case BRGT:
            {
               if (greater())
               {
                  pc = op1;
               }
               break;
            }
            case BRLE:
            {
               if (lower() || (equal()))
               {
                  pc = op1;
               }
               break;
            }
            case BRLT:
            {
               if (lower())
               {
                  pc = op1;
               }
               break;
            }
            case CALL:
            {
               stack_push(pc);
               pc = op1;
               break;
            }
            case RET:
            {
               stack_pop(&pc);
               break;
            }
            case RETI:
            {
               return_from_interrupt();
               break;
            }
            case PUSH:
            {
               stack_push(reg[op1]);
               break;
            }
            case POP:
            {
               stack_pop(&reg[op1]);
               break;
            }
            case SEI:
            {
               set(sr, I);
               break;
            }
            case CLI:
            {
               clr(sr, 4);
               break;
            }
            default:
            {
               control_unit_reset();
               break;
            }
         }

         state = CPU_STATE_FETCH;      /* Fetches next instruction during next clock cycle. */
         break;
      }
      default:
      {
         control_unit_reset();         /* System reset if error occurs. */
         break;
      }
   }

   monitor_interrupts();               /* Monitors interrupts during every clock cycle. */
   return;
}

/********************************************************************************
* control_unit_run_next_state: Runs next CPU instruction cycle, i.e. fetches
*                              a new instruction from program memory, decodes
*                              and executes it.
********************************************************************************/
void control_unit_run_next_instruction_cycle(void)
{
   do
   {
      control_unit_run_next_state();
   } while (state != CPU_STATE_EXECUTE);
   return;
}

/********************************************************************************
* control_unit_print: Prints information about the processor, for instance
*                     current subroutine, instruction, state, content in
*                     CPU-registers and I/O registers DDRB, PORTB and PINB.
********************************************************************************/
void control_unit_print(void)
{
   printf("--------------------------------------------------------------------------------\n");
   printf("Current subroutine:\t\t\t\t%s\n", program_memory_subroutine_name(mar));
   printf("Current instruction:\t\t\t\t%s\n", cpu_instruction_name(op_code));
   printf("Current state:\t\t\t\t\t%s\n", cpu_state_name(state));
   
   printf("Program counter:\t\t\t\t%hu\n", pc);

   printf("Instruction register:\t\t\t\t%s ", get_binary((ir >> 16) & 0xFF, 8));
   printf("%s ", get_binary((ir >> 8) & (0xFF), 8));
   printf("%s\n", get_binary(ir & 0xFF, 8));

   printf("Status register (INZVC):\t\t\t%s\n\n", get_binary(sr, 5));

   printf("Content in CPU register R16:\t\t\t%s\n", get_binary(reg[R16], 8));
   printf("Content in CPU register R24:\t\t\t%s\n\n", get_binary(reg[R24], 8));

   printf("Content in data direction register DDRB:\t%s\n", get_binary(data_memory_read(DDRB), 8));
   printf("Content in data register PORTB:\t\t\t%s\n", get_binary(data_memory_read(PORTB), 8));
   printf("Content in pin input register PINB:\t\t%s\n", get_binary(data_memory_read(PINB), 8));

   printf("--------------------------------------------------------------------------------\n\n");
   return;
}

static inline void cpu_registers_reset(void)
{
   for (uint8_t* i = reg; i < reg + CPU_REGISTER_ADDRESS_WIDTH; ++i)
   {
      *i = 0x00;
   }
   return;
}

static inline bool interrupt_enabled(void)
{
   return read(sr, I);
}

static inline bool equal(void)
{
   return read(sr, Z);
}

static inline bool greater(void)
{
   return (!equal() && !lower());
}

static inline bool lower(void)
{
   return read(sr, N);
}

static inline void monitor_interrupts(void)
{
   pci_regs_monitor_pci_interrupt_on_io_port(&pci_regs_b);
   pci_regs_monitor_pci_interrupt_on_io_port(&pci_regs_c);
   pci_regs_monitor_pci_interrupt_on_io_port(&pci_regs_d);
   return;
}

static void generate_interrupt(const uint8_t interrupt_vector, 
                               const uint8_t flag_bit)
{
   clr(sr, I);

   stack_push(pc);
   stack_push(mar);
   stack_push(sr);

   stack_push(ir << 16);
   stack_push(ir << 8);
   stack_push(ir);

   stack_push(op_code);
   stack_push(op1);
   stack_push(op2);

   stack_push(state);
   stack_push(flag_bit);

   for (uint8_t i = 0; i < CPU_REGISTER_DATA_WIDTH; ++i)
   {
      stack_push(reg[i]);
   }

   pc = interrupt_vector;
   return;
}

static void return_from_interrupt(void)
{
   uint8_t flag_bit = 0x00;
   uint8_t interrupt_vector = 0x00;
   uint8_t temp = 0x00;

   for (uint8_t i = CPU_REGISTER_DATA_WIDTH; i > 0; --i)
   {
      stack_pop(&reg[i - 1]);
   }

   stack_pop(&flag_bit);
   stack_pop(&temp);
   state = (enum cpu_state)(temp);

   stack_pop(&op2);
   stack_pop(&op1);
   stack_pop(&op_code);

   stack_pop(&temp);
   ir = temp;
   stack_pop(&temp);
   ir |= temp << 8;
   stack_pop(&temp);
   ir |= temp << 16;

   stack_pop(&sr);
   stack_pop(&mar);
   stack_pop(&pc);

   temp = data_memory_read(PCIFR);
   clr(temp, flag_bit);
   data_memory_write(PCIFR, temp);

   set(sr, I);
   return;
}
