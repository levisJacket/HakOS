
[bits 16]

MEMORY_MAP_BUFFER equ 0x7E00

memory_size_start:
    mov ax, ds
    mov es, ax

    mov di, MEMORY_MAP_BUFFER
    xor ebx, ebx
    mov word [entry_count], 0

memory_size_loop:
    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 24

    mov dword [es:di + 20], 1

    int 0x15

    jc check_finished 

    cmp eax, 0x534D4150     
    jne failed 

    jcxz skip_entry           

    cmp ecx, 20               
    jbe process_entry        

    test dword [es:di + 20], 1
    jz skip_entry 

process_entry:
    mov eax, [es:di + 8]
    or  eax, [es:di + 12]
    jz  skip_entry

    inc word [entry_count]
    add di, 24

skip_entry:
    test ebx, ebx                
    jz finished
    jmp memory_size_loop 

check_finished:
    cmp word [entry_count], 0
    jne finished

failed:
    stc                       
    ret

finished:
    clc
    ret

entry_count dw 0 

