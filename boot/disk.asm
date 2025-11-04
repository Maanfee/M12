; =============================================================================
; Disk Load Routine
; Loads sectors from disk into memory using BIOS INT 13h
; 
; Input:
;   DL = Drive number (0x80 for first HDD, 0x00 for first FDD)
;   DH = Number of sectors to read
;   ES:BX = Memory address to load sectors to
; =============================================================================
disk_load:
    pusha                   ; Save all general-purpose registers
    
    ; Set up disk read parameters for BIOS INT 13h, AH=02h
    mov ah, 0x02            ; BIOS read sector function
    mov al, dh              ; Number of sectors to read (from input DH)
    mov cl, 0x02            ; Start from sector 2 (sector 1 is boot sector)
    xor ch, ch              ; Clear CH (cylinder number = 0) - using XOR for efficiency
    xor dh, dh              ; Clear DH (head number = 0) - using XOR for efficiency
    ; DL already contains drive number from caller
    
    ; Call BIOS disk service
    int 0x13
    jc disk_error           ; Jump if carry flag set (error occurred)
    
    ; Verify that the correct number of sectors were read
    ; [ESP + 10] accesses the original DH value from the PUSHA stack frame
    cmp al, [esp + 10]      ; Compare sectors read (AL) with sectors requested
    jne sectors_error       ; Jump if incorrect number of sectors read
    
    popa                    ; Restore all general-purpose registers
    ret                     ; Return to caller

; =============================================================================
; Disk Error Handler
; Called when BIOS indicates a disk read error (carry flag set)
; =============================================================================
disk_error:
    ; Store error code for debugging purposes
    ; BIOS returns error code in AH register
    mov byte [DISK_ERROR_CODE], ah
    
    ; Infinite loop - system halted due to disk error
    jmp disk_loop

; =============================================================================
; Sectors Error Handler  
; Called when incorrect number of sectors were read
; =============================================================================
sectors_error:
    ; Infinite loop - system halted due to sector count mismatch
    jmp disk_loop

; =============================================================================
; System Halt Loop
; Infinite loop that halts the system when fatal errors occur
; =============================================================================
disk_loop:
    jmp $                   ; Jump to self (infinite loop)

; =============================================================================
; Data Section
; =============================================================================
DISK_ERROR_CODE: db 0       ; Storage for disk error code from BIOS
