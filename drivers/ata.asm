global ata_read_sector
global ata_write_sector

ata_read_sector:
    push edi
    mov eax, [esp + 8] ; eax = LBA 
    mov edi, [esp + 12] ; edi = memory address

    push edx
    push ecx
    push eax

    ; select drive & top 4 bits of LBA:
    ; Port 0x1F6: 0xE0 (Master drive, LBA mode) | bits 24-27 of LBA
    mov edx, 0x1F6
    shr eax, 24
    or al, 0xE0 ; 0b11100000
    out dx, al

    ; sector count = 1:
    mov edx, 0x1F2
    mov al, 1
    out dx, al

    ;send LBA bits 0-7 to 0x1F3:
    pop eax                  ; Restore original LBA
    push eax
    mov edx, 0x1F3
    out dx, al

    ;bits 8-15 to 0x1F4:
    mov edx, 0x1F4
    shr eax, 8
    out dx, al

    ;bits 16-23 to 0x1F5:
    mov edx, 0x1F5
    shr eax, 8
    out dx, al

    ; send READ SECTORS 0x20 to 0x1F7:
    mov edx, 0x1F7
    mov al, 0x20
    out dx, al

    ; poll status 0x1F7 until READY:
.read_wait_ready:
    in al, dx
    test al, 0x80            ; bit 7 (BSY - busy): must be 0
    jnz .read_wait_ready
    test al, 0x08            ; bit 3 (DRQ - data request): must be 1
    jz .read_wait_ready

    ; read 256 words (512 bytes) from data port 0x1F0 into [EDI]:
    mov edx, 0x1F0
    mov ecx, 256             ; 256 words
    rep insw                 ; stream sector into RAM

    pop eax
    pop ecx
    pop edx
    pop edi
    ret

ata_write_sector:
    push esi
    mov eax, [esp + 8] ; eax = LBA 
    mov esi, [esp + 12] ; edi = memory address

    push edx
    push ecx
    push eax

    mov edx, 0x1F6
    shr eax, 24
    or al, 0xE0
    out dx, al

    mov edx, 0x1F2
    mov al, 1
    out dx, al

    pop eax
    push eax
    mov edx, 0x1F3
    out dx, al

    mov edx, 0x1F4
    shr eax, 8
    out dx, al

    mov edx, 0x1F5
    shr eax, 8
    out dx, al

    mov edx, 0x1F7
    mov al, 0x30 	;write sectors
    out dx, al

.write_wait_ready:
    in al, dx
    test al, 0x80           
    jnz .write_wait_ready
    test al, 0x08          
    jz .write_wait_ready

    mov edx, 0x1F0
    mov ecx, 256             
    rep outsw         	;stream from RAM
    
    mov al, 0xE7
    out 0x1F7, al

    pop eax
    pop ecx
    pop edx
    pop esi
    ret
