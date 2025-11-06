#include "../include/isr.h"
#include "../include/common.h"
#include "../include/idt.h"

// نام exceptionها
static const char* exception_messages[32] = {
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void isr_handler(InterruptFrame* frame) {
	kprintf("\n=== EXCEPTION ===\n");
	kprintf("Exception: %s\n", exception_messages[frame->interrupt_number]);
	kprintf("Error Code: 0x%x\n", frame->error_code);
	kprintf("RIP: 0x%x\n", frame->rip);
	kprintf("CS: 0x%x\n", frame->cs);
	kprintf("RFLAGS: 0x%x\n", frame->rflags);
	kprintf("================\n");

	// اگر exception بحرانی بود، سیستم را متوقف کن
	if (frame->interrupt_number == 8 ||  // Double Fault
		frame->interrupt_number == 13 || // General Protection Fault
		frame->interrupt_number == 14) { // Page Fault
		uint64_t fault_address;
		asm volatile("mov %%cr2, %0" : "=r"(fault_address));

		kprintf("Page Fault at address: 0x%x\n", fault_address);
		kprintf("Error Code: 0x%x\n", frame->error_code);

		// اطلاعات بیشتر درباره علت خطا
		if (frame->error_code & 0x1) {
			kprintf("Cause: Page protection violation\n");
		}
		else {
			kprintf("Cause: Page not present\n");
		}
	}
}

void Init_ISR(void) {
	// تنظیم IDT entries برای exceptionها
	// استفاده مستقیم از توابع isrX
	idt_set_entry(0, (uint64_t)isr0, 0x08, 0x8E);
	idt_set_entry(1, (uint64_t)isr1, 0x08, 0x8E);
	idt_set_entry(2, (uint64_t)isr2, 0x08, 0x8E);
	idt_set_entry(3, (uint64_t)isr3, 0x08, 0x8E);
	idt_set_entry(4, (uint64_t)isr4, 0x08, 0x8E);
	idt_set_entry(5, (uint64_t)isr5, 0x08, 0x8E);
	idt_set_entry(6, (uint64_t)isr6, 0x08, 0x8E);
	idt_set_entry(7, (uint64_t)isr7, 0x08, 0x8E);
	idt_set_entry(8, (uint64_t)isr8, 0x08, 0x8E);
	idt_set_entry(9, (uint64_t)isr9, 0x08, 0x8E);
	idt_set_entry(10, (uint64_t)isr10, 0x08, 0x8E);
	idt_set_entry(11, (uint64_t)isr11, 0x08, 0x8E);
	idt_set_entry(12, (uint64_t)isr12, 0x08, 0x8E);
	idt_set_entry(13, (uint64_t)isr13, 0x08, 0x8E);
	idt_set_entry(14, (uint64_t)isr14, 0x08, 0x8E);
	idt_set_entry(15, (uint64_t)isr15, 0x08, 0x8E);
	idt_set_entry(16, (uint64_t)isr16, 0x08, 0x8E);
	idt_set_entry(17, (uint64_t)isr17, 0x08, 0x8E);
	idt_set_entry(18, (uint64_t)isr18, 0x08, 0x8E);
	idt_set_entry(19, (uint64_t)isr19, 0x08, 0x8E);
	idt_set_entry(20, (uint64_t)isr20, 0x08, 0x8E);
	idt_set_entry(21, (uint64_t)isr21, 0x08, 0x8E);
	idt_set_entry(22, (uint64_t)isr22, 0x08, 0x8E);
	idt_set_entry(23, (uint64_t)isr23, 0x08, 0x8E);
	idt_set_entry(24, (uint64_t)isr24, 0x08, 0x8E);
	idt_set_entry(25, (uint64_t)isr25, 0x08, 0x8E);
	idt_set_entry(26, (uint64_t)isr26, 0x08, 0x8E);
	idt_set_entry(27, (uint64_t)isr27, 0x08, 0x8E);
	idt_set_entry(28, (uint64_t)isr28, 0x08, 0x8E);
	idt_set_entry(29, (uint64_t)isr29, 0x08, 0x8E);
	idt_set_entry(30, (uint64_t)isr30, 0x08, 0x8E);
	idt_set_entry(31, (uint64_t)isr31, 0x08, 0x8E);

	DEBUGER(LIGHT_RED, "ISR initialized - %d exception handlers registered", 32);
}

