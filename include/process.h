#ifndef PROCESS_H
#define PROCESS_H

#include "type.h"

#define MAX_PROCESSES 64
#define KERNEL_STACK_SIZE 4096
#define USER_STACK_SIZE 4096

// وضعیت‌های پردازش
typedef enum {
    PROCESS_UNUSED = 0,
    PROCESS_RUNNING,
    PROCESS_READY, 
    PROCESS_BLOCKED,
    PROCESS_ZOMBIE
} process_state_t;

// زمینه (Context) پردازش برای ذخیره registerها
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
    uint64_t rip, cs, rflags, rsp, ss;
} process_context_t;

// ساختار پردازش (PCB)
typedef struct process {
    uint32_t pid;                   // شناسه پردازش
    process_state_t state;          // وضعیت پردازش
    process_context_t context;      // زمینه پردازش
    uint64_t* page_table;           // جدول صفحه پردازش
    uint64_t kernel_stack;          // پشته کرنل
    uint64_t user_stack;            // پشته کاربر
    uint32_t exit_code;             // کد خروج
    struct process* parent;         // پردازش والد - تغییر به struct process*
    uint64_t brk;                   // break pointer برای heap
} process_t;

// توابع مدیریت پردازش
void InitProcessManager(void);
process_t* create_process(void (*entry_point)(void));
void schedule(void);
extern void switch_process(process_context_t* old, process_context_t* new);
uint32_t get_current_pid(void);
process_t* get_current_process(void);

// سیستم‌کال‌های مدیریت پردازش
uint64_t sys_fork_handler(void);
uint64_t sys_exec_handler(const char* path, char* const argv[]);
uint64_t sys_exit_handler_real(uint32_t status);
uint64_t sys_wait_handler(int* status);
uint64_t sys_getpid_handler_real(void);
uint64_t sys_brk_handler(void* addr);

#endif