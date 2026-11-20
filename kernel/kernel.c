#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../cpu/irq.h"
#include "../cpu/idt.h"
#include "memory.h"
#include "process.h"

#include "../user/shell.h"
#include "../user/dummy.h"
#include "../drivers/ata.h"

char mem_bitmap[1024];

typedef struct {
    uint32_t memory_map_addr;
} boot_info_t;

void main(boot_info_t info) {
    clear_screen();

    isr_install();
    irq_install();

    pmm_init(info.memory_map_addr, mem_bitmap);

    process_clear();
    int proc1 = process_create(shell_loop);
    int proc2 = process_create(dummyProc);

    uint32_t* page_to_read_hdd = pmm_alloc();
    ata_read_sector(0, page_to_read_hdd);
    print_long(*page_to_read_hdd);
    (*page_to_read_hdd) = (*page_to_read_hdd) + 1;
    print_long(*page_to_read_hdd);
    ata_write_sector(0, page_to_read_hdd);

    asm volatile("int $16");
    while(1) {
	process_cleanup();
    }
}

