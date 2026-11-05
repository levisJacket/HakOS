#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../cpu/irq.h"
#include "../cpu/idt.h"
#include "memory.h"
#include "process.h"

char mem_bitmap[1024];

char charmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void print_long(uint32_t val){
    char to_print[9];
    to_print[8] = 0;
    for (int i = 0; i < 8 ; i++){
	to_print[7-i] = charmap[val & 15];
	val = val >> 4; 
    }
    print(to_print);
}

void printHello();
void printBye();

typedef struct {
    uint32_t memory_map_addr;
} boot_info_t;

void main(boot_info_t info) {
    clear_screen();

    isr_install();
    irq_install();

    pmm_init(info.memory_map_addr, mem_bitmap);

    clear_process();
    process_t* proc1 = create_process(printHello);
    process_t* proc2 = create_process(printBye);

    asm volatile("int $16");
    print_long((uint32_t)proc1);
    print_long((uint32_t)proc2);
    while(1) {
	char toprint[] = {'M', 'a', 'i', 'n', '\n', 0};
	print(toprint);
	for(int i = 0; i < 1000000; i++){}
    }
}
 
void printHello(){
    char toprint[] = {'H', 'e', 'l', 'l', 'o', '\n', 0};
    while (1) {
	print(toprint);
	for(int i = 0; i < 1000000; i++){}
    }
}

void printBye(){
    char toprint[] = {'B','y', 'e', '\n', 0};
    while (1) {
	print(toprint);
	for(int i = 0; i < 1000000; i++){}
    }
}
