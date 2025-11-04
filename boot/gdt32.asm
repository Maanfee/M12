[bits 16]

gdt_start32:
    dq 0x0

gdt_code32:
    dw 0xFFFF       ; Limit 0-15
    dw 0x0000       ; Base 0-15  
    db 0x00         ; Base 16-23
    db 0x9A         ; P=1, DPL=0, S=1, Type=Code, Execute/Read
    db 0xCF         ; G=1, D=1, L=0, Limit 16-19
    db 0x00         ; Base 24-31

gdt_data32:
    dw 0xFFFF       ; Limit 0-15
    dw 0x0000       ; Base 0-15
    db 0x00         ; Base 16-23
    db 0x92         ; P=1, DPL=0, S=1, Type=Data, Read/Write
    db 0xCF         ; G=1, D=1, L=0, Limit 16-19
    db 0x00         ; Base 24-31

gdt_end32:

gdt_descriptor32:
    dw gdt_end32 - gdt_start32 - 1
    dd gdt_start32

CODE_SEG32 equ gdt_code32 - gdt_start32
DATA_SEG32 equ gdt_data32 - gdt_start32