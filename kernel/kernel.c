#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../cpu/irq.h"
#include "../cpu/idt.h"

void main() {
    char message[10];
    clear_screen();

    print("\n");
    isr_install();
    irq_install();
    __asm__ __volatile__("int $16");
}

