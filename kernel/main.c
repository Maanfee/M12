#include "../include/common.h"
#include "../include/idt.h"
#include "../include/isr.h"
#include "../include/irq.h"
#include "../include/syscall.h"
#include "../include/memory.h"
#include "../include/paging.h"

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
	//// تست mapping آدرس‌های مختلف
	//DEBUGER(LIGHT_CYAN, "Testing page mappings...");

	//// تست identity mapping
	//uint64_t test_phys = 0x200000;  // 2MB
	//uint64_t test_virt = 0x200000;

	//DEBUGER(LIGHT_CYAN, "Identity mapping: VA 0x%x -> PA 0x%x", test_virt, test_phys);

	//// تست kernel higher half mapping
	//uint64_t kernel_virt = KERNEL_VIRTUAL_BASE + 0x1000;
	//uint64_t kernel_phys = 0x1000;

	//DEBUGER(LIGHT_CYAN, "Kernel mapping: VA 0x%x -> PA 0x%x", kernel_virt, kernel_phys);

	//// نمایش اطلاعات page table
	//void* pml4 = get_kernel_page_table();
	//DEBUGER(LIGHT_CYAN, "Kernel PML4 at: 0x%x", pml4);

	//DEBUGER(LIGHT_GREEN, "Paging test completed");

//	print_memory_info();


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
