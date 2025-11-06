[bits 64]

section .text

; „«ò—Ê »—«?  ⁄—?› exception handler »œÊ‰ òœ Œÿ«
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0                 ; push error code dummy
    push %1                ; push interrupt number
    jmp isr_common_stub
%endmacro

; „«ò—Ê »—«?  ⁄—?› exception handler »« òœ Œÿ«
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push %1                ; push interrupt number
    jmp isr_common_stub
%endmacro

;  ⁄—?› exception handlers
ISR_NOERRCODE 0    ; Divide Error
ISR_NOERRCODE 1    ; Debug
ISR_NOERRCODE 2    ; NMI
ISR_NOERRCODE 3    ; Breakpoint
ISR_NOERRCODE 4    ; Overflow
ISR_NOERRCODE 5    ; Bound Range
ISR_NOERRCODE 6    ; Invalid Opcode
ISR_NOERRCODE 7    ; Device Not Available
ISR_ERRCODE   8    ; Double Fault
ISR_NOERRCODE 9    ; Coprocessor Segment Overrun
ISR_ERRCODE   10   ; Invalid TSS
ISR_ERRCODE   11   ; Segment Not Present
ISR_ERRCODE   12   ; Stack-Segment Fault
ISR_ERRCODE   13   ; General Protection Fault
ISR_ERRCODE   14   ; Page Fault
ISR_NOERRCODE 15   ; Reserved
ISR_NOERRCODE 16   ; x87 Floating-Point Exception
ISR_ERRCODE   17   ; Alignment Check
ISR_NOERRCODE 18   ; Machine Check
ISR_NOERRCODE 19   ; SIMD Floating-Point Exception
ISR_NOERRCODE 20   ; Virtualization Exception
ISR_NOERRCODE 21   ; Reserved
ISR_NOERRCODE 22   ; Reserved
ISR_NOERRCODE 23   ; Reserved
ISR_NOERRCODE 24   ; Reserved
ISR_NOERRCODE 25   ; Reserved
ISR_NOERRCODE 26   ; Reserved
ISR_NOERRCODE 27   ; Reserved
ISR_NOERRCODE 28   ; Reserved
ISR_NOERRCODE 29   ; Reserved
ISR_NOERRCODE 30   ; Reserved
ISR_NOERRCODE 31   ; Reserved

; Common ISR stub
extern isr_handler
isr_common_stub:
    ; –Œ?—Â  „«„ registerÂ«
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; ›—«ŒÊ«‰? handler œ— C
    mov rdi, rsp    ; «Ê·?‰ ¬—êÊ„«‰: «‘«—Âùê— »Â InterruptFrame
    call isr_handler
    
    ; »«“?«»? registerÂ«
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Å«ò ò—œ‰ interrupt number Ê error code
    add rsp, 16
    
    iretq           ; »«“ê‘  «“ interrupt
