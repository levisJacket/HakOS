#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>

#define PAGE_SIZE       4096

void pmm_init(uint32_t memory_map_addr, void* bitmap_buffer);

void* pmm_alloc();

void pmm_free_page(void* ptr);

uint32_t* create_process_directory();

void map_page(uint32_t *dir, uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);

uint32_t* get_kernel_page_directory();

#endif
