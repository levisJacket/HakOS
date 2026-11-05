#pragma once
#include <stdint.h>

#define PROCESS_UNUSED 0
#define PROCESS_READY 1
#define PROCESS_RUNNING 2
#define PROCESS_SLEEPING 3
#define PROCESS_ZOMBIE 4

typedef struct process {
    int pid;
    int state;
    uint32_t esp;
    uint32_t* page_directory;
    struct process* next;
} process_t;

void clear_process();
process_t* create_process(void (*entry_point)());
void destroy_process(int pid);

process_t* schedule(uint32_t current_esp);
