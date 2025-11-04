; =============================================================================
; Switch to Protected Mode Routine
; Transitions the processor from 16-bit real mode to 32-bit protected mode
; =============================================================================
[bits 16]
switch_to_pm:
    ; Disable interrupts - critical during mode transition
    ; Interrupts must be disabled before changing to protected mode because:
    ; 1. Real mode interrupt vectors are incompatible with protected mode
    ; 2. The IDTR needs to be properly set up for protected mode interrupts
    cli
    
    ; Load Global Descriptor Table (GDT)
    ; The GDT defines the memory segments and their privileges in protected mode
    ; gdt_descriptor32 contains the size and address of the GDT
    lgdt [gdt_descriptor32]
    
    ; Enable protected mode by setting the Protection Enable (PE) bit in CR0
    ; CR0 is the control register that controls processor operating modes
    mov eax, cr0           ; Read current CR0 value
    or eax, 0x1            ; Set bit 0 (PE bit) to 1
    mov cr0, eax           ; Write back to CR0 - processor now in protected mode
    
    ; Far jump to flush the processor pipeline and reload CS with 32-bit code segment
    ; This is CRITICAL because:
    ; 1. The processor still has real mode instructions in the pipeline
    ; 2. CS needs to be loaded with the protected mode code segment selector
    ; 3. CODE_SEG32 is the offset into the GDT for the 32-bit code segment
    jmp CODE_SEG32:init_pm

; =============================================================================
; Protected Mode Initialization
; This code executes in 32-bit protected mode
; =============================================================================
[bits 32]
init_pm:
    ; Set up protected mode stack
    ; Stack grows downward from 0x90000 to lower addresses
    ; Using 0x9000 as stack pointer provides ample stack space (36KB available)
    ; Note: In some implementations, this might be 0x90000 (check your memory map)
    mov esp, 0x9000        ; Set stack pointer for protected mode
    
    ; Jump to the main protected mode code
    ; begin_pm is typically where the main 32-bit OS initialization begins
    jmp begin_pm