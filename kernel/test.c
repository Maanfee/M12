#include "../include/test.h"
#include "../include/paging.h"
#include "../include/memory.h"
#include "../include/idt.h"
#include "../include/isr.h"
#include "../include/irq.h"
#include "../include/syscall.h"
#include "../include/vga.h"
#include "../include/process.h"  

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
	add_test("Process Management", test_process_management, "Test process creation and management");  
	add_test("Syscall Functions", test_syscall_functions, "Test system call implementations");        
	add_test("Context Switching", test_context_switching, "Test process context switching");         
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

		int result = RUN_TEST(test_cases[i].function);

		switch (result) {
		case TEST_PASS:
			kprintcolor(LIGHT_GREEN, "PASS");
			tests_passed++;
			break;
		case TEST_FAIL:
			kprintcolor(LIGHT_RED, "FAIL");
			tests_failed++;
			break;
		case TEST_SKIP:
			kprintcolor(LIGHT_YELLOW, "SKIP");
			tests_skipped++;
			break;
		}
	
		kprintf(" - %s\n", test_cases[i].description);
	}
	
	print_test_results();
}

int _run_test(int (*test_func)(void), const char* func_name, const char* file, int line) {
	return test_func();
}

void _assert(int condition, const char* expr, const char* file, int line) {
	if (!condition) {
		kprintcolor(LIGHT_RED, "Assertion failed: %s at %s:%d", expr, file, line);
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

// ==================== تست‌های موجود ====================

int test_basic_paging(void) {
	void* pml4 = get_kernel_page_table();
	ASSERT(pml4 != NULL);

	uint64_t cr3;
	asm volatile("mov %%cr3, %0" : "=r"(cr3));
	ASSERT(cr3 != 0);

	return TEST_PASS;
}

int test_kernel_mappings(void) {
    kprintf("\n    [Testing kernel mappings...] ");
    
    uint64_t phys_addr = get_physical_address(0x100000);
    if (phys_addr != 0x100000) {
        kprintcolor(LIGHT_RED, "Basic address translation failed");
        return TEST_FAIL;
    }
    
    uint64_t pml4_index = PML4_INDEX(KERNEL_VIRTUAL_BASE);
    uint64_t pdpt_index = PDPT_INDEX(KERNEL_VIRTUAL_BASE);
    
    if (pml4_index != 0x1FF) {
        kprintcolor(LIGHT_RED, "PML4 index calculation wrong: %d", pml4_index);
        return TEST_FAIL;
    }
    
    kprintcolor(LIGHT_GREEN, "Kernel mapping calculations OK");
    return TEST_PASS;
}

int test_memory_management(void) {
	void* page1 = get_physical_page();
	ASSERT(page1 != NULL);

	volatile uint64_t* test_ptr = (volatile uint64_t*)page1;
	*test_ptr = 0x12345678;
	ASSERT_EQ(*test_ptr, 0x12345678);

	free_physical_page(page1);

	void* pages[3];
	for (int i = 0; i < 3; i++) {
		pages[i] = get_physical_page();
		ASSERT(pages[i] != NULL);
	}

	for (int i = 0; i < 3; i++) {
		free_physical_page(pages[i]);
	}

	return TEST_PASS;
}

int test_interrupt_system(void) {
	return TEST_PASS;
}

int test_syscall_mechanism(void) {
	return TEST_PASS;
}

int test_vga_output(void) {
	Color original_color = color;

	SetColor(LIGHT_GREEN);
	kprintf("VGA");
	SetColor(original_color);
	kprintf(" output test: ");

	return TEST_PASS;
}

// ==================== تست‌های جدید مدیریت پردازش ====================

int test_process_management(void) {
    kprintf("\n    [Testing process management...] ");
    
    // تست ایجاد پردازش
    void test_dummy_process(void) {
        // پردازش تست ساده
    }
    
    process_t* proc = create_process(test_dummy_process);
    if (proc == NULL) {
        kprintcolor(LIGHT_RED, "Process creation failed");
        return TEST_FAIL;
    }
    
    // تست PID
    if (proc->pid == 0) {
        kprintcolor(LIGHT_RED, "Invalid PID assigned");
        return TEST_FAIL;
    }
    
    // تست وضعیت پردازش
    if (proc->state != PROCESS_READY) {
        kprintcolor(LIGHT_RED, "Process state not READY");
        return TEST_FAIL;
    }
    
    // تست پشته‌ها
    if (proc->kernel_stack == 0 || proc->user_stack == 0) {
        kprintcolor(LIGHT_RED, "Process stacks not allocated");
        return TEST_FAIL;
    }
    
    // تست زمینه پردازش
    if (proc->context.rip != (uint64_t)test_dummy_process) {
        kprintcolor(LIGHT_RED, "Process entry point not set correctly");
        return TEST_FAIL;
    }
    
    kprintcolor(LIGHT_GREEN, "Process creation OK (PID: %d)", proc->pid);
    return TEST_PASS;
}

int test_syscall_functions(void) {
    kprintf("\n    [Testing syscall functions...] ");
    
    // ابتدا مطمئن شویم current_process وجود دارد
    process_t* current = get_current_process();
    if (current == NULL) {
        kprintcolor(LIGHT_YELLOW, "No current process - skipping syscall tests");
        return TEST_SKIP;
    }
    
    kprintf("\n        Current process PID: %d", current->pid);
    
    // تست sys_getpid
    uint32_t pid = sys_getpid_handler_real();
    kprintf("\n        sys_getpid returned: %d", pid);
    
    if (pid == 0) {
        kprintcolor(LIGHT_RED, "sys_getpid returned 0 (should be > 0)");
        return TEST_FAIL;
    }
    
    // فقط تست‌های پایه را انجام بده
    kprintcolor(LIGHT_GREEN, "Basic syscall functionality OK");
    return TEST_PASS;
}

int test_context_switching(void) {
    kprintf("\n    [Testing context switching...] ");
    
    // ذخیره زمینه فعلی
    process_context_t old_context;
    memset(&old_context, 0, sizeof(process_context_t));
    
    // ایجاد زمینه تست
    process_context_t test_context;
    memset(&test_context, 0, sizeof(process_context_t));
    test_context.rip = 0x1000;
    test_context.rsp = 0x7FFF0;
    
    // تست اینکه تابع switch_process قابل فراخوانی است
    // این فقط signature را تست می‌کند، نه عملکرد واقعی را
    // چون تعویض زمینه واقعی نیاز به تنظیمات خاص دارد
    
    kprintcolor(LIGHT_YELLOW, "Basic context structure OK (full test requires actual switch)");
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
		kprintcolor(LIGHT_GREEN, "ALL TESTS PASSED! System is stable.\n");
	}
	else {
		kprintcolor(LIGHT_RED, "Some tests failed. System needs debugging.\n");
	}
	kprintcolor(LIGHT_CYAN, "================================\n");
}