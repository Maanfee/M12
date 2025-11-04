[org 0x7C00]
[bits 16]

; Bootloader Entry Point
_start:
    ; Initialize segment registers to 0
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x8000          ; Set stack pointer (combining bp and sp)
    
    ; Save boot drive number provided by BIOS in DL
    mov [BOOT_DRIVE], dl

    ; Load kernel from disk to memory address 0x10000 (0x1000:0x0000)
    mov ax, 0x1000          ; ES:BX = 0x1000:0x0000 = physical 0x10000
    mov es, ax
    xor bx, bx              ; Zero BX using XOR (smaller/faster than MOV)
    mov dh, 128             ; Number of sectors to read (64KB)
    mov dl, [BOOT_DRIVE]    ; Drive number
    call disk_load          ; Load kernel from disk
    
    call switch_to_pm       ; Switch to 32-bit protected mode

    ; Should never reach here
    jmp $

; Include necessary modules
%include "disk.asm"     ; Disk I/O routines
%include "pmode.asm"    ; Protected mode switching
%include "gdt32.asm"    ; 32-bit Global Descriptor Table

; Data section
BOOT_DRIVE: db 0        ; Storage for boot drive number

; =============================================================================
; 32-bit Protected Mode Entry Point
; =============================================================================
[bits 32]
begin_pm:
    ; Initialize segment registers with data segment selector
    mov ax, DATA_SEG32
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up stack
    mov ebp, 0x9000
    mov esp, ebp

    ; =========================================================================
    ; Page Table Setup for Long Mode
    ; =========================================================================
    
    ; Initialize Page Map Level 4 Table (PML4) at 0x1000
    mov edi, 0x1000        ; PML4 table address
    mov cr3, edi           ; Set CR3 to point to PML4
    
    ; Clear page tables (4KB = 4096 bytes = 1024 DWORDs)
    xor eax, eax           ; Zero EAX
    mov ecx, 1024          ; 4096 bytes / 4 bytes per DWORD
    rep stosd              ; Clear memory using DWORD stores (faster than BYTE)
    
    ; Reset EDI to PML4 base
    mov edi, 0x1000
    
    ; Set up page table hierarchy:
    ; PML4[0] -> PDPT at 0x2000
    ; PDPT[0] -> PDT at 0x3000  
    ; PDT[0] -> PT at 0x4000
    mov DWORD [edi], 0x2003        ; PML4[0]: Points to PDPT, Present + Writeable
    mov DWORD [edi + 0x1000], 0x3003  ; PDPT[0]: Points to PDT, Present + Writeable
    mov DWORD [edi + 0x2000], 0x4003  ; PDT[0]: Points to PT, Present + Writeable
    
    ; Initialize Page Table entries to identity map first 2MB
    mov ebx, 0x00000003    ; Physical address + Present + Writeable flags
    mov ecx, 512           ; 512 entries * 4KB = 2MB mapping
    
.SetEntry:
    mov DWORD [edi + 0x3000], ebx  ; Set page table entry
    add ebx, 0x1000        ; Next 4KB page
    add edi, 8             ; Next 8-byte entry
    loop .SetEntry

    ; =========================================================================
    ; Enable Long Mode
    ; =========================================================================
    
    ; Enable Physical Address Extension (PAE)
    mov eax, cr4
    or eax, (1 << 5)       ; Set PAE bit (bit 5)
    mov cr4, eax

    ; Enable Long Mode in EFER MSR
    mov ecx, 0xC0000080    ; EFER MSR number
    rdmsr                  ; Read EFER
    or eax, (1 << 8)       ; Set LME bit (bit 8)
    wrmsr                  ; Write EFER

    ; Enable Paging and Protected Mode simultaneously
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0)  ; PG (bit 31) + PE (bit 0)
    mov cr0, eax

    ; Load 64-bit GDT and jump to 64-bit code segment
    lgdt [gdt64.Pointer]
    jmp gdt64.Code:begin_lm

; Include 64-bit GDT definitions
%include "gdt64.asm"

; =============================================================================
; 64-bit Long Mode Entry Point
; =============================================================================
[bits 64]
begin_lm:
    ; Set up 64-bit stack
    mov rsp, 0x9000
    
    ; Display "OS" in green on black background at top-left of screen
    ; VGA text mode format: [attribute][character]
    mov dword [0xb8000], 0x2f532f4f  ; "OS" in green

    ; Jump to loaded kernel at 0x10000
    jmp 0x10000

; Boot signature
times 510-($-$$) db 0
dw 0xAA55