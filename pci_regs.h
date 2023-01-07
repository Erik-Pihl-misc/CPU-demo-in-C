#ifndef PCI_REGS_H_
#define PCI_REGS_H_

#include "control_unit.h"

struct pci_regs
{
   const uint8_t pin_reg;
   const uint8_t mask_reg;
   const uint8_t flag_bit;
   const uint8_t interrupt_vector;
   uint8_t last_value;
   const struct pci_regs_vtable* vptr;
};

struct pci_regs_vtable
{
   bool (*interrupt_enabled)(void);
   void (*generate_interrupt)(const uint8_t interrupt_vector, 
                              const uint8_t flag_bit);
};

static inline void pci_regs_monitor_pci_interrupt_on_io_port(struct pci_regs* self);
static inline bool pci_regs_pin_change_detected(const struct pci_regs* self);
static void pci_regs_check_pin_event(struct pci_regs* self);
static void pci_regs_check_for_interrupt_request(const struct pci_regs* self,
                                                 const uint8_t bit);
static inline void pci_regs_set_interrupt_flag(const struct pci_regs* self);

static inline void pci_regs_monitor_pci_interrupt_on_io_port(struct pci_regs* self)
{
   if (pci_regs_pin_change_detected(self))
   {
      pci_regs_check_pin_event(self);
   }
   return;
}

static inline bool pci_regs_pin_change_detected(const struct pci_regs* self)
{
   if (self->last_value != data_memory_read(self->pin_reg))
   {
      return true;
   }
   else
   {
      return false;
   }
}

static void pci_regs_check_pin_event(struct pci_regs* self)
{
   const uint8_t current_value = data_memory_read(self->pin_reg);

   for (uint8_t i = 0; i < IO_REGISTER_DATA_WIDTH; ++i)
   {
      if (read(current_value, i) != read(self->last_value, i))
      {
         pci_regs_check_for_interrupt_request(self, i);
      }
   }

   self->last_value = current_value;
   return;
}

static void pci_regs_check_for_interrupt_request(const struct pci_regs* self,
                                                 const uint8_t bit)
{                 
   const uint8_t mask_reg_content = data_memory_read(self->mask_reg);

   if (read(mask_reg_content, bit))
   {
      pci_regs_set_interrupt_flag(self);

      if (self->vptr->interrupt_enabled())
      {
         self->vptr->generate_interrupt(self->interrupt_vector, self->flag_bit);
      }
   }
   return;
}

static inline void pci_regs_set_interrupt_flag(const struct pci_regs* self)
{
   uint8_t flag_reg_content = data_memory_read(PCIFR);
   set(flag_reg_content, self->flag_bit);
   data_memory_write(PCIFR, flag_reg_content);
   return;
}

#endif /* PCI_REGS_H_ */