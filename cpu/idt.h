#ifndef IDT_H
#define IDT_H
#include <stdint.h>

#define KERNEL_CS 0x08

// 1. One 8-byte IDT gate descriptor
typedef struct {
    uint16_t low_offset;  // Lower 16 bits of handler function address
    uint16_t sel;         // Kernel Code Segment selector (0x08)
    uint8_t  always0;     // Unused byte (always 0)
    uint8_t  flags;       // 0x8E (32-bit Interrupt Gate, Ring 0, Present)
    uint16_t high_offset; // Higher 16 bits of handler function address
} __attribute__((packed)) idt_gate_t;

// 2. The IDT pointer structure passed to the 'lidt' instruction
typedef struct {
    uint16_t limit;       // Size of table in bytes - 1 (256 * 8 - 1)
    uint32_t base;        // 32-bit base address of the IDT array
} __attribute__((packed)) idt_register_t;

// 64bytes total
typedef struct {
    uint32_t ds;                                     // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;                       
    uint32_t eip, cs, eflags, useresp, ss;           // Pushed by the processor automatically
} registers_t;

#define IDT_ENTRIES 256
extern idt_gate_t idt[IDT_ENTRIES];
extern idt_register_t idt_reg;

void set_idt_gate(int n, uint32_t handler);
void set_idt();
#endif
