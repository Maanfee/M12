[bits 64]

section .text

global switch_process
switch_process:
    ; Save registers of current process
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    
    ; Save RSP in old context (rdi points to old context)
    mov [rdi], rsp
    
    ; Load new RSP from new context (rsi points to new context)
    mov rsp, [rsi]
    
    ; Restore registers of new process
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    
    ret