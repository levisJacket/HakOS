#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "memory.h"

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t acpi_attr;
} __attribute__((packed)) e820_entry_t;

static uint32_t* pmm_bitmap = NULL;
static size_t    total_pages = 0;
static size_t    bitmap_size = 0; // Size in bytes
static uint32_t* kernel_page_directory = NULL;
			    
#define PAGE_SIZE       4096
#define PAGE_ALIGN_UP(x)   	(((x) + PAGE_SIZE - 1) 	& ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_DOWN(x)   	((x) 			& ~(PAGE_SIZE - 1))

#define BITMAP_SET(page)   (pmm_bitmap[(page) / 32] |=  (1U << ((page) % 32)))
#define BITMAP_CLEAR(page) (pmm_bitmap[(page) / 32] &= ~(1U << ((page) % 32)))
#define BITMAP_TEST(page)  (pmm_bitmap[(page) / 32] &   (1U << ((page) % 32)))

void pmm_init(uint32_t memory_map_addr, void* bitmap_buffer) {
    long memory_top = 0;
    e820_entry_t* mem_map = (e820_entry_t*)(memory_map_addr);
    for(int i=0; i<20; i++){
	uint32_t top = (uint32_t)mem_map[i].length + (uint32_t)mem_map[i].base_addr;
	if (top > memory_top) memory_top = top;
    }
    
    total_pages = memory_top / PAGE_SIZE;
    bitmap_size = (total_pages + 7) / 8; // 1 byte handles 8 pages
    
    pmm_bitmap = (uint32_t*)bitmap_buffer;
    for (uint32_t i = 0; i < (total_pages + 31) / 32; i++) {
	pmm_bitmap[i] = 0xFFFFFFFF;
    }

    for(int i=0; i<20; i++){
	uint32_t base_low = (uint32_t)mem_map[i].base_addr;
	uint32_t length_low = (uint32_t)mem_map[i].length;
	uint32_t type = mem_map[i].type;
	
	if (type == 1 && base_low >= 0x100000) {
	    int j = PAGE_ALIGN_UP(base_low) / PAGE_SIZE;
	    int end_page = PAGE_ALIGN_DOWN(base_low + length_low) / PAGE_SIZE;
	    for (; j < end_page ; j++) {
		BITMAP_CLEAR(j);
	    }
	}
    }

    kernel_page_directory = (uint32_t*)pmm_alloc();
    uint32_t* page_table = (uint32_t*)pmm_alloc();

    //map the kernel (1 table, 4MB)
    kernel_page_directory[0] = ((uint32_t)page_table) | 3;
    
    // Fill the other 1023 directory slots as Not Present
    for (int i = 1; i < 1024; i++) {
        kernel_page_directory[i] = 0x00000002; // R/W=1, Present=0
    }

    for (int i = 0; i < 1024; i++){
	page_table[i] = (i*0x1000) | 3; //present bit 1 & RW bit 2 
    }

    asm volatile(
        "movl %0, %%cr3" 
	: 
	: "r" (kernel_page_directory) 
	: "memory"
    );
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" 
	: "=r"(cr0)
    );
    cr0 |= 0x80000000; 
    asm volatile("mov %0, %%cr0" 
	: 
	: "r"(cr0)
    );
}

void* pmm_alloc() {
    for (int i = 0; i < bitmap_size ; i++) {
	if (pmm_bitmap[i] == 0xFFFFFFFF) { continue; }
	for (int bit = 0; bit < 32; bit++){
	    if (!(pmm_bitmap[i] & (1U << bit))) {
		uint32_t frame_index = (i * 32) + bit;
		if (frame_index >= total_pages)	{ return NULL; }

		BITMAP_SET(frame_index);

		return (void*)(frame_index * PAGE_SIZE);
	    }
	}
    }
}

void pmm_free_page(void* ptr) {
    uint32_t addr = (uint32_t)ptr;

    if (addr % PAGE_SIZE != 0) { return; }

    uint32_t frame_index = addr / PAGE_SIZE;
    if (frame_index >= total_pages) { return; }

    // Mark bit as 0 (Free)
    BITMAP_CLEAR(frame_index);
}

uint32_t* create_process_directory() {
    uint32_t* proc_dir = (uint32_t*)pmm_alloc();
    if (!proc_dir) { return NULL; }

    proc_dir[0] = kernel_page_directory[0];

    for (int i = 1; i < 1024; i++){
	proc_dir[i] = 0x00000002;
    }

    return proc_dir;
}

void destroy_process_directory(uint32_t* proc_dir){
    for (int i = 1; i < 1024; i++){
	pmm_free_page((void*)proc_dir[i]);
    }
    pmm_free_page(proc_dir);
};

void map_page(uint32_t *dir, uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags) {
    uint32_t pd_index = virtual_addr >> 22;          // Top 10 bits
    uint32_t pt_index = (virtual_addr >> 12) & 0x3FF;// Middle 10 bits

    uint32_t *page_table;

    // if no page table for this 4MB region, create one
    if (!(dir[pd_index] & 1)) {
	page_table = (uint32_t*)pmm_alloc();
	for (int i = 0; i < 1024; i++) { page_table[i] = 0x00000002; }

	// Wire into Page Directory (User=1, R/W=1, Present=1)
	dir[pd_index] = ((uint32_t)page_table) | flags | 3;
    } else {
	page_table = (uint32_t*)(dir[pd_index] & ~0xFFF);
    }

    // Map physical frame to page table
    page_table[pt_index] = (physical_addr & ~0xFFF) | flags | 1;
}

uint32_t* get_kernel_page_directory(){
    return kernel_page_directory;
}

