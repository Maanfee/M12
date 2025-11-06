#include "../include/idt.h"
#include "../include/common.h"

static IDTEntry idt[IDT_ENTRIES];
static IDTPointer idt_ptr;

void idt_set_entry(uint8_t index, uint64_t offset, uint16_t selector, uint8_t type_attr) {
    idt[index].offset_low = offset & 0xFFFF;
    idt[index].selector = selector;
    idt[index].ist = 0;
    idt[index].type_attr = type_attr;
    idt[index].offset_mid = (offset >> 16) & 0xFFFF;
    idt[index].offset_high = (offset >> 32) & 0xFFFFFFFF;
    idt[index].zero = 0;
}

void Init_IDT(void) {
    // Initialize IDT pointer
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;

    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_entry(i, 0, 0, 0);
    }

    // Load IDT
    asm volatile("lidt %0" : : "m"(idt_ptr));

    DEBUGER(LIGHT_RED, "IDT initialized at: 0x%x", &idt);
}