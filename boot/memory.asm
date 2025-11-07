[bits 16]

MEMORY_MAP_LOCATION equ 0x5000

detect_memory:
    pusha
    push es
    push di
    
    mov ax, 0x0000
    mov es, ax
    mov di, MEMORY_MAP_LOCATION + 2
    
    xor ebx, ebx
    mov edx, 0x534D4150
    mov ecx, 24
    
    mov dword [entry_count], 0
    
.memory_loop:
    mov eax, 0xE820
    int 0x15
    jc .memory_done
    
    cmp eax, 0x534D4150
    jne .memory_error
    
    ; Increase entry count
    inc dword [entry_count]
    add di, cx
    
    test ebx, ebx
    jz .memory_done
    
    ; Limit to 32 entries
    cmp dword [entry_count], 32
    jb .memory_loop

.memory_done:
    ; Save entry count
    mov eax, [entry_count]
    mov di, MEMORY_MAP_LOCATION
    mov [di], eax
    
    pop di
    pop es
    popa
    ret

.memory_error:
    mov di, MEMORY_MAP_LOCATION
    mov dword [di], 0
    pop di
    pop es
    popa
    ret

entry_count: dd 0
