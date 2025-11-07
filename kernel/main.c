#include "../include/common.h"
#include "../include/idt.h"
#include "../include/isr.h"
#include "../include/irq.h"
#include "../include/syscall.h"
#include "../include/memory.h"
#include "../include/paging.h"
#include "../include/test.h"

void _kmain() {
	Init_VGA();
	Init_Memory();
	Init_Paging();
	Init_IDT();
	Init_ISR();
	Init_IRQ();
	Init_Syscall();

	sti();
	OS;

	MemoryInfo();
	
	// اجرای تست‌های یکپارچگی
	init_test_suite();
	run_all_tests();

	kprintf("\n" SYSTEM_READY_MESSAGE);

	// =============================================================
	//int number = 10;    
	//int* ptr;          
	//ptr = &number;     
	//kprintf("number: %d\n", number);     // خروجی: 10
	//kprintf("&number: %p\n", &number); // خروجی: یک آدرس هگز (مثلاً 0x7ffd42a1a234)
	//kprintf("ptr: %p\n", ptr); // خروجی: همان آدرس بالا
	//kprintf("*ptr: %d\n", *ptr); // خروجی: 10
	// =============================================================
	// number = *ptr || &number = ptr

	//char* str2 = "World";
   /* char* ptr = str2;
	while (*ptr != '\0') {
		kprintf("%c \n", *ptr);
		ptr++;
	}*/

	/*int length = strlen("World");
	kprintf("%d \n", length);*/

	// =================== End ===================

	while (1) {
		hlt();
	}
}
