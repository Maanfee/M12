#include "../include/vga.h"
#include "../include/common.h"
#include "../include/kernel.h"

void _kmain() {

	Init_VGA();
	OS;

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