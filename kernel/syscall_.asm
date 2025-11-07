[bits 64]

section .text

;  ⁄—?› syscall_handler »Â ⁄‰Ê«‰ external
extern syscall_handler

;  ⁄—?› kernel stack œ— section .bss
section .bss
align 16
kernel_stack_bottom:
    resb 4096           ; 4KB stack »—«? kernel
kernel_stack_top:

section .text

global syscall_entry
syscall_entry:
    ; –Œ?—Â registerÂ«
    push rcx    ; –Œ?—Â RIP
    push r11    ; –Œ?—Â RFLAGS
    
    ; »«—ê–«—? segment registers »—«? kernel
    mov rcx, 0x10  ; kernel data segment
    mov ds, rcx
    mov es, rcx
    mov fs, rcx
    mov gs, rcx
    
    ; –Œ?—Â stack pointer ò«—»—
    push rsp
    mov rsp, kernel_stack_top
    
    ; ¬„«œÂ ”«“? ¬—êÊ„«‰ùÂ« »—«? handler œ— C
    ; syscall_handler(syscall_num, arg1, arg2, arg3, arg4, arg5)
    mov rdi, rax        ; syscall_num («“ rax)
    mov rsi, rbx        ; arg1 («“ rbx)
    mov rdx, rdx        ; arg2 («“ rdx - »œÊ‰  €??—)
    mov rcx, rcx        ; arg3 («“ rcx - »œÊ‰  €??—)
    mov r8, r8          ; arg4 («“ r8)
    mov r9, r9          ; arg5 («“ r9)
    
    ; ›—«ŒÊ«‰? handler œ— C
    call syscall_handler
    
    ; »«“ê—œ«‰? stack pointer ò«—»—
    pop rsp
    
    ; »«“ê—œ«‰? registerÂ«
    pop r11             ; »«“ê—œ«‰? RFLAGS
    pop rcx             ; »«“ê—œ«‰? RIP
    
    ; »«“ê‘  «“ ”?” „ùò«·
    o64 sysret
