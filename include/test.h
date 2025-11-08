#ifndef TEST_H
#define TEST_H

#include "type.h"
#include "common.h"

// وضعیت‌های تست
#define TEST_PASS 0
#define TEST_FAIL 1
#define TEST_SKIP 2

// ساختار تست
typedef struct {
    const char* name;
    int (*function)(void);
    const char* description;
} test_case_t;

// ماکروهای تست
#define RUN_TEST(test_func) _run_test(test_func, #test_func, __FILE__, __LINE__)
#define ASSERT(condition) _assert((condition), #condition, __FILE__, __LINE__)
#define ASSERT_EQ(a, b) _assert_eq((a), (b), #a, #b, __FILE__, __LINE__)
#define ASSERT_NEQ(a, b) _assert_neq((a), (b), #a, #b, __FILE__, __LINE__)

// توابع تست
void init_test_suite(void);
void run_all_tests(void);
void add_test(const char* name, int (*func)(void), const char* desc);
int _run_test(int (*test_func)(void), const char* func_name, const char* file, int line);
void _assert(int condition, const char* expr, const char* file, int line);
void _assert_eq(uint64_t a, uint64_t b, const char* a_str, const char* b_str, const char* file, int line);
void _assert_neq(uint64_t a, uint64_t b, const char* a_str, const char* b_str, const char* file, int line);

// تست‌های خاص
int test_basic_paging(void);
int test_kernel_mappings(void);
int test_memory_management(void);
int test_interrupt_system(void);
int test_syscall_mechanism(void);
int test_vga_output(void);
int test_process_management(void);      
int test_syscall_functions(void);       
int test_context_switching(void);        

// گزارش نتایج
void print_test_results(void);

#endif
