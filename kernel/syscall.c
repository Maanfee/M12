#include "../include/syscall.h"
#include "../include/common.h"
#include "../include/vga.h"
#include "../include/process.h"

// آرایه‌ای از توابع سیستم‌کال
static uint64_t(*syscall_table[SYSCALL_COUNT])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// Handler اصلی سیستم‌کال
uint64_t syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    DEBUGER(LIGHT_CYAN, "Syscall %d called with args: 0x%x, 0x%x, 0x%x",
        syscall_num, arg1, arg2, arg3);

    if (syscall_num >= SYSCALL_COUNT) {
        DEBUGER(LIGHT_RED, "Invalid syscall number: %d", syscall_num);
        return -1;
    }

    if (syscall_table[syscall_num] == NULL) {
        DEBUGER(LIGHT_RED, "Unimplemented syscall: %d", syscall_num);
        return -1;
    }

    // فراخوانی تابع سیستم‌کال مربوطه
    return syscall_table[syscall_num](arg1, arg2, arg3, arg4, arg5);
}

// پیاده‌سازی سیستم‌کال‌های ساده
static uint64_t sys_write_handler(uint64_t fd, uint64_t buffer, uint64_t count, uint64_t arg4, uint64_t arg5) {
    char* str = (char*)buffer;
    for (size_t i = 0; i < count && str[i] != '\0'; i++) {
        VGA_Putc(str[i]);
    }
    return count;
}

// سیستم‌کال‌های ساده دیگر
static uint64_t sys_dummy_handler(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    DEBUGER(LIGHT_RED, "Dummy syscall called");
    return 0;
}

// wrapper functions برای تطبیق signature
static uint64_t sys_fork_wrapper(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    return sys_fork_handler();
}

static uint64_t sys_exec_wrapper(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    return sys_exec_handler((const char*)arg1, (char* const*)arg2);
}

static uint64_t sys_exit_wrapper(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    return sys_exit_handler_real((uint32_t)arg1);
}

static uint64_t sys_wait_wrapper(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    return sys_wait_handler((int*)arg1);
}

static uint64_t sys_getpid_wrapper(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    return sys_getpid_handler_real();
}

static uint64_t sys_brk_wrapper(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    return sys_brk_handler((void*)arg1);
}

void Init_Syscall(void) {
    // مقداردهی اولیه جدول سیستم‌کال
    for (int i = 0; i < SYSCALL_COUNT; i++) {
        syscall_table[i] = sys_dummy_handler;
    }

    // ثبت سیستم‌کال‌های پیاده‌سازی شده
    syscall_table[SYS_EXIT] = sys_exit_wrapper;
    syscall_table[SYS_WRITE] = sys_write_handler;
    syscall_table[SYS_GETPID] = sys_getpid_wrapper;
    syscall_table[SYS_FORK] = sys_fork_wrapper;
    syscall_table[SYS_EXEC] = sys_exec_wrapper;
    syscall_table[SYS_WAIT] = sys_wait_wrapper;
    syscall_table[SYS_BRK] = sys_brk_wrapper;

    // تنظیم MSRهای سیستم‌کال
    uint64_t star_msr = ((uint64_t)0x08 << 32) | ((uint64_t)0x10 << 48);
    asm volatile("wrmsr" : : "c"(0xC0000081), "a"(star_msr & 0xFFFFFFFF), "d"(star_msr >> 32));

    // تنظیم entry point سیستم‌کال
    asm volatile("wrmsr" : : "c"(0xC0000082), "a"((uint64_t)syscall_entry & 0xFFFFFFFF), "d"((uint64_t)syscall_entry >> 32));

    // فعال کردن سیستم‌کال
    uint64_t efer_msr;
    asm volatile("rdmsr" : "=a"(efer_msr) : "c"(0xC0000080));
    efer_msr |= 1;
    asm volatile("wrmsr" : : "c"(0xC0000080), "a"(efer_msr & 0xFFFFFFFF), "d"(efer_msr >> 32));

    DEBUGER(LIGHT_RED, "Syscall initialized - %d syscalls registered", SYSCALL_COUNT);
}
