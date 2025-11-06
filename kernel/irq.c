#include "../include/irq.h"
#include "../include/vga.h"
#include "../include/common.h"
#include "../include/idt.h"
#include "../include/asm.h"

// آرایه‌ای از handlerهای IRQ
static void (*irq_handlers[IRQ_COUNT])(InterruptFrame*);

void irq_handler(InterruptFrame* frame) {
    uint8_t irq_num = frame->interrupt_number - 32;

    // اگر handler نصب شده، فراخوانی کن
    if (irq_handlers[irq_num] != NULL) {
        irq_handlers[irq_num](frame);
    }

    // Send EOI to PIC
    if (irq_num >= 8) {
        outb(0xA0, 0x20);  // EOI to slave PIC
    }
    outb(0x20, 0x20);      // EOI to master PIC
}

void irq_install_handler(int irq, void (*handler)(InterruptFrame*)) {
    if (irq >= 0 && irq < IRQ_COUNT) {
        irq_handlers[irq] = handler;
    }
}

void irq_uninstall_handler(int irq) {
    if (irq >= 0 && irq < IRQ_COUNT) {
        irq_handlers[irq] = NULL;
    }
}

// Initialize PIC
void Init_PIC(void) {
    // Initialize master PIC
    outb(0x20, 0x11);
    outb(0x21, 0x20);
    outb(0x21, 0x04);
    outb(0x21, 0x01);

    // Initialize slave PIC
    outb(0xA0, 0x11);
    outb(0xA1, 0x28);
    outb(0xA1, 0x02);
    outb(0xA1, 0x01);

    // Mask all interrupts
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void Init_IRQ(void) {
    // Initialize PIC
    Init_PIC();

    // Clear IRQ handlers
    for (int i = 0; i < IRQ_COUNT; i++) {
        irq_handlers[i] = NULL;
    }

    // Install تمام IRQ entries در IDT
    idt_set_entry(32, (uint64_t)irq0, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(33, (uint64_t)irq1, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(34, (uint64_t)irq2, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(35, (uint64_t)irq3, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(36, (uint64_t)irq4, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(37, (uint64_t)irq5, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(38, (uint64_t)irq6, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(39, (uint64_t)irq7, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(40, (uint64_t)irq8, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(41, (uint64_t)irq9, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(42, (uint64_t)irq10, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(43, (uint64_t)irq11, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(44, (uint64_t)irq12, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(45, (uint64_t)irq13, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(46, (uint64_t)irq14, 0x08, IDT_TYPE_INTERRUPT);
    idt_set_entry(47, (uint64_t)irq15, 0x08, IDT_TYPE_INTERRUPT);

    DEBUGER(LIGHT_RED, "IRQ initialized - %d IRQ handlers registered", IRQ_COUNT);
}
