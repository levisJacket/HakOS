#include <stddef.h>
#include "process.h"
#include "memory.h"

#include "../drivers/screen.h"

#define MAX_PROCESS 16
process_t pcb_list[MAX_PROCESS];
process_t *current_process = &pcb_list[0];
static int next_pid = 1;


static process_t* proc_alloc(){
    for (int i = 0; i < MAX_PROCESS; i++){
	if (!pcb_list[i].state){
	    return &pcb_list[i];
	}
    }
    return NULL;
}

static inline void proc_free(process_t* proc){
    proc->state = PROCESS_UNUSED;
}

static process_t* proc_find(int pid){
    for(int i = 0; i < MAX_PROCESS; i++){
	if (pcb_list[i].pid == pid) return &pcb_list[i];
    }
}

static void proc_unlink(process_t* proc){
    asm volatile("cli");
    for (int i = 0; i < MAX_PROCESS; i++){
	if (pcb_list[i].next == proc) {
	    pcb_list[i].next = proc->next;
	    break;
	}
    }
    asm volatile("sti");
}

void process_clear(){
    for (int i = 0; i < MAX_PROCESS; i++){
	pcb_list[i].state = PROCESS_UNUSED;
    }
    process_t* proc = &pcb_list[0];
    
    proc->pid = 0;
    proc->state = PROCESS_RUNNING;
    proc->page_directory = get_kernel_page_directory();
    proc->next = proc;
}

static void process_exit(){
    current_process->state = PROCESS_ZOMBIE;

    asm volatile("int $32");
    while(1) { } 
}

int process_create(void (*entry_point)()){
    process_t* proc = proc_alloc();

    if (!proc) { return -1; }

    uint32_t* proc_dir = create_process_directory();
    uint32_t* page_table = pmm_alloc();
    uint32_t* stack_frame = pmm_alloc();

    proc_dir[1] = ((uint32_t)page_table | 3);
    page_table[0] = ((uint32_t)stack_frame | 3);

    uint32_t* stack = (uint32_t*)((uint32_t)stack_frame + PAGE_SIZE);

    *(--stack) = (uint32_t)process_exit;	// return when done
    *(--stack) = 0x0202;                // EFLAGS (IF = 1)
    *(--stack) = 0x08;                  // CS
    *(--stack) = (uint32_t)entry_point; // EIP

    *(--stack) = 0;                     // Error code
    *(--stack) = 32;                    // Interrupt number
    *(--stack) = 0;                     // EDI
    *(--stack) = 0;                     // ESI
    *(--stack) = 0;                     // EBP
    *(--stack) = 0;                     // ESP
    *(--stack) = 0;                     // EBX
    *(--stack) = 0;                     // EDX
    *(--stack) = 0;                     // ECX
    *(--stack) = 0;                     // EAX
    *(--stack) = 0x10;                  // DS (Data segment)

    proc->esp = (uint32_t)0x400000 + PAGE_SIZE - (15 * sizeof(uint32_t));
    proc->pid = next_pid++;
    proc->state = PROCESS_READY;
    proc->page_directory = proc_dir;
    asm volatile("cli");
    proc->next = pcb_list[0].next;
    pcb_list[0].next = proc;
    asm volatile("sti");

    print("new process on : ");
    print_long((int)proc->page_directory);

    return proc->pid;
}

static void proc_clean(process_t* proc){
    proc_unlink(proc);

    uint32_t* proc_dir = proc->page_directory;

    for(int i = 1; i < 0x400; i++){
	if (proc_dir[i] & 1) {
	    uint32_t* page_table = (uint32_t*)(proc_dir[i] & ~0xFFF);
	    for (int j = 0; j < 0x400; j++){
		if (page_table[j] & 1) {
		    pmm_free_page( (void*)(page_table[j] & ~0xFFF) );
		}
	    }
	    pmm_free_page( (uint32_t*)page_table);
	}
    }
    pmm_free_page(proc_dir);
    proc_free(proc);
}

void process_cleanup(){
    for (int i = 0; i < MAX_PROCESS; i++){
	if (pcb_list[i].state == PROCESS_ZOMBIE) {
	    proc_clean(&pcb_list[i]);
	}
    }
}

void schedule(uint32_t current_esp) {
    if (current_process->state == PROCESS_RUNNING) {
	current_process->state = PROCESS_READY;
    }

    do {
	current_process = current_process->next;
    } while (current_process->state != PROCESS_READY);

    current_process->state = PROCESS_RUNNING;
    print("switched to: ");
    print_long(current_process->pid);
}
