#include "irq.h"
#include "idt.h"
#include "../kernel/low_level.h"
#include "../kernel/process.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
extern process_t *current_process;


uint8_t tick;

void pic_remap(){
    uint8_t mask1, mask2;

    mask1 = port_byte_in(0x21);
    mask2 = port_byte_in(0xA1);

    port_byte_out(0x20, 0x10 | 0x01);
    port_byte_out(0xA0, 0x10 | 0x01);

    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);

    port_byte_out(0x21, 4);
    port_byte_out(0xA1, 2);

    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);

    port_byte_out(0x21, mask1);
    port_byte_out(0xA1, mask2);

    //port_byte_out(0x21, 0xFD); 
    //port_byte_out(0xA1, 0xFF);
}

void irq_handler(registers_t* r) {
    current_process->esp = (uint32_t)r;

    if (r->int_no == 32) {
	tick++;
	if(!(tick & 15)){
	    schedule((uint32_t)r);
	}
    }

    if (r->int_no == 33) {
	uint8_t scancode = port_byte_in(0x60);
	keyboard_callback(scancode);
    }

    if (r->int_no >= 40) {
	port_byte_out(0xA0, 0x20); //Slave
    }
    port_byte_out(0x20, 0x20);     //Master
}   

void irq_install() {
    pic_remap();
    
    set_idt_gate(32, (uint32_t)irq0);
    set_idt_gate(33, (uint32_t)irq1);

    __asm__ __volatile__("sti");
}
