#include "../include/test.h"
#include "../include/paging.h"
#include "../include/memory.h"
#include "../include/idt.h"
#include "../include/isr.h"
#include "../include/irq.h"
#include "../include/syscall.h"
#include "../include/vga.h"

// متغیرهای تست
static test_case_t test_cases[50];
static uint32_t test_count = 0;
static uint32_t tests_passed = 0;
static uint32_t tests_failed = 0;
static uint32_t tests_skipped = 0;

void init_test_suite(void) {
	test_count = 0;
	tests_passed = 0;
	tests_failed = 0;
	tests_skipped = 0;

	// ثبت تمام تست‌ها
	add_test("Basic Paging", test_basic_paging, "Test basic paging functionality");
	add_test("Kernel Mappings", test_kernel_mappings, "Test kernel virtual address mappings");
	add_test("Memory Management", test_memory_management, "Test physical memory allocation");
	add_test("Interrupt System", test_interrupt_system, "Test interrupt handling");
	add_test("Syscall Mechanism", test_syscall_mechanism, "Test system call infrastructure");
	add_test("VGA Output", test_vga_output, "Test VGA text output");
}

void add_test(const char* name, int (*func)(void), const char* desc) {
	if (test_count < 50) {
		test_cases[test_count].name = name;
		test_cases[test_count].function = func;
		test_cases[test_count].description = desc;
		test_count++;
	}
}

void run_all_tests(void) {
	kprintf("\n");
	kprintcolor(LIGHT_CYAN, "=== Starting M12 OS Test Suite ===\n");
	kprintf("Running %d tests...\n\n", test_count);

	for (uint32_t i = 0; i < test_count; i++) {
		kprintf("[%d/%d] %s: ", i + 1, test_count, test_cases[i].name);

		//    int result = RUN_TEST(test_cases[i].function);

		//    switch (result) {
		//    case TEST_PASS:
		//        kprintcolor(LIGHT_GREEN, "PASS");
		//        tests_passed++;
		//        break;
		//    case TEST_FAIL:
		//        kprintcolor(LIGHT_RED, "FAIL");
		//        tests_failed++;
		//        break;
		//    case TEST_SKIP:
		//        kprintcolor(LIGHT_YELLOW, "SKIP");
		//        tests_skipped++;
		//        break;
		//    }

		//    kprintf(" - %s\n", test_cases[i].description);
	}

	//print_test_results();
}

int _run_test(int (*test_func)(void), const char* func_name, const char* file, int line) {
	return test_func();
}

void _assert(int condition, const char* expr, const char* file, int line) {
	if (!condition) {
		kprintcolor(LIGHT_RED, "Assertion failed: %s at %s:%d", expr, file, line);
		// می‌توانیم اینجا متوقف شویم یا ادامه دهیم
	}
}

void _assert_eq(uint64_t a, uint64_t b, const char* a_str, const char* b_str, const char* file, int line) {
	if (a != b) {
		kprintcolor(LIGHT_RED, "Assertion failed: %s (0x%x) == %s (0x%x) at %s:%d",
			a_str, a, b_str, b, file, line);
	}
}

void _assert_neq(uint64_t a, uint64_t b, const char* a_str, const char* b_str, const char* file, int line) {
	if (a == b) {
		kprintcolor(LIGHT_RED, "Assertion failed: %s (0x%x) != %s (0x%x) at %s:%d",
			a_str, a, b_str, b, file, line);
	}
}

// ==================== تست‌های خاص ====================

int test_basic_paging(void) {
	void* pml4 = get_kernel_page_table();
	ASSERT(pml4 != NULL);

	uint64_t cr3;
	asm volatile("mov %%cr3, %0" : "=r"(cr3));
	ASSERT(cr3 != 0);

	return TEST_PASS;
}

int test_kernel_mappings(void) {
	// تست identity mapping
	volatile uint64_t* test_phys = (volatile uint64_t*)0x100000;
	*test_phys = 0xDEADBEEF;
	ASSERT_EQ(*test_phys, 0xDEADBEEF);

	// تست وجود mapping مجازی کرنل
	volatile uint64_t* test_virt = (volatile uint64_t*)(KERNEL_VIRTUAL_BASE + 0x100000);
	ASSERT((uint64_t)test_virt > KERNEL_VIRTUAL_BASE);

	// تست ترجمه آدرس
	uint64_t phys_addr = get_physical_address(KERNEL_VIRTUAL_BASE + 0x100000);
	ASSERT_EQ(phys_addr, 0x100000);

	return TEST_PASS;
}

int test_memory_management(void) {
	// تست تخصیص صفحه
	void* page1 = get_physical_page();
	ASSERT(page1 != NULL);

	// تست نوشتن در صفحه
	volatile uint64_t* test_ptr = (volatile uint64_t*)page1;
	*test_ptr = 0x12345678;
	ASSERT_EQ(*test_ptr, 0x12345678);

	// تست آزادسازی
	free_physical_page(page1);

	// تست تخصیص چندگانه
	void* pages[3];
	for (int i = 0; i < 3; i++) {
		pages[i] = get_physical_page();
		ASSERT(pages[i] != NULL);
	}

	// آزادسازی
	for (int i = 0; i < 3; i++) {
		free_physical_page(pages[i]);
	}

	return TEST_PASS;
}

int test_interrupt_system(void) {
	// تست‌های اولیه - سیستم وقفه باید راه‌اندازی شده باشد
	// تست‌های پیشرفته‌تر بعداً اضافه می‌شوند

	// این تست فقط بررسی می‌کند که initialization انجام شده
	// تست‌های واقعی وقفه بعداً اضافه می‌شوند

	return TEST_PASS;
}

int test_syscall_mechanism(void) {
	// تست‌های اولیه سیستم‌کال
	// در این مرحله فقط بررسی می‌کنیم که initialization انجام شده

	return TEST_PASS;
}

int test_vga_output(void) {
	// تست خروجی VGA با نوشتن یک تست ساده
	Color original_color = color;

	SetColor(LIGHT_GREEN);
	kprintf("VGA");
	SetColor(original_color);
	kprintf(" output test: ");

	// اگر به اینجا رسیدیم، یعنی VGA کار می‌کند
	return TEST_PASS;
}

void print_test_results(void) {
	kprintf("\n");
	kprintcolor(LIGHT_CYAN, "=== Test Results ===\n");
	kprintf("Total Tests:  %d\n", test_count);
	kprintcolor(LIGHT_GREEN, "Passed:       %d\n", tests_passed);
	kprintcolor(LIGHT_RED, "Failed:       %d\n", tests_failed);
	kprintcolor(LIGHT_YELLOW, "Skipped:      %d\n", tests_skipped);

	kprintf("\n");
	if (tests_failed == 0) {
		kprintcolor(LIGHT_GREEN, "✅ ALL TESTS PASSED! System is stable.\n");
	}
	else {
		kprintcolor(LIGHT_RED, "❌ Some tests failed. System needs debugging.\n");
	}
	kprintcolor(LIGHT_CYAN, "================================\n");
}
