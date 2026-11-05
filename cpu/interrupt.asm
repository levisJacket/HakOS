[bits 32]
[extern isr_handler]
[extern irq_handler]
[extern current_process]


isr_common_stub:
    pusha                    ; Push edi, esi, ebp, esp, ebx, edx, ecx, eax

    mov ax, ds               ; Lower 16-bits of eax = ds.
    push eax                 ; Save the data segment descriptor

    mov ax, 0x10             ; Load kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp                 ; Pass stack frame pointer to C
    call isr_handler
    add esp, 4               ; Clean up passed parameter

    pop eax                  ; Restore original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                     ; Restore general purpose registers
    add esp, 8               ; Clean up pushed error code and pushed ISR number
    iret                     ; Pop CS, EIP, EFLAGS, SS, and ESP & go back
irq_common_stub:
    pusha
    mov ax, ds
    push eax

    mov ax, 0x10             ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp                 ; Pass registers_t* to C
    call irq_handler

    mov ebx, [current_process]      

    mov eax, [ebx + 12]         ; cr3 = current_process->page_directory
    mov cr3, eax 

    mov esp, [ebx + 8] 		; esp = current_process->page_directory

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8               ; Clean error code and interrupt number
    iret


%macro ISR_NOERRCODE 1
[global isr%1]
isr%1:
    push byte 0         ; Dummy error code
    push byte %1        ; Interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
[global isr%1]
isr%1:
    push byte %1        ; Interrupt number
    jmp isr_common_stub
%endmacro

%assign irq_num 0
%assign int_num 32
%rep 16
[global irq%[irq_num]]
irq%[irq_num]:
    push byte 0                ; Dummy error code for uniform stack frame
    push byte int_num          ; Interrupt number (32 - 47)
    jmp irq_common_stub

%assign irq_num irq_num + 1
%assign int_num int_num + 1
%endrep


ISR_NOERRCODE 0   ; Divide by Zero
ISR_NOERRCODE 1   ; Debug
ISR_NOERRCODE 2   ; Non-Maskable Interrupt
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound Range Exceeded
ISR_NOERRCODE 6   ; Invalid Opcode
ISR_NOERRCODE 7   ; Device Not Available
ISR_ERRCODE   8   ; Double Fault
ISR_NOERRCODE 9   ; Coprocessor Segment Overrun
ISR_ERRCODE   10  ; Invalid TSS 
ISR_ERRCODE   11  ; Segment Not Present
ISR_ERRCODE   12  ; Stack-Segment Fault 
ISR_ERRCODE   13  ; General Protection Fault 
ISR_ERRCODE   14  ; Page Fault
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 Floating-Point Exception
ISR_ERRCODE   17  ; Alignment Check 
ISR_NOERRCODE 18  ; Machine Check
ISR_NOERRCODE 19  ; SIMD Floating-Point Exception
ISR_NOERRCODE 20  ; Virtualization Exception
ISR_NOERRCODE 21  ; Reserved
ISR_NOERRCODE 22  ; Reserved
ISR_NOERRCODE 23  ; Reserved
ISR_NOERRCODE 24  ; Reserved
ISR_NOERRCODE 25  ; Reserved
ISR_NOERRCODE 26  ; Reserved
ISR_NOERRCODE 27  ; Reserved
ISR_NOERRCODE 28  ; Reserved
ISR_NOERRCODE 29  ; Reserved
ISR_ERRCODE   30  ; Security Exception 
ISR_NOERRCODE 31  ; Reserved

