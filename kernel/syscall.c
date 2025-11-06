#include "../include/syscall.h"
#include "../include/common.h"
#include "../include/idt.h"

// آرایه‌ای از توابع سیستم‌کال
static uint64_t(*syscall_table[SYSCALL_COUNT])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// Handler اصلی سیستم‌کال - باید global باشد
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
static uint64_t sys_exit_handler(uint64_t status, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    DEBUGER(LIGHT_RED, "Process exit with status: %d", status);
    // TODO: پیاده‌سازی خروج فرایند
    return 0;
}

static uint64_t sys_write_handler(uint64_t fd, uint64_t buffer, uint64_t count, uint64_t arg4, uint64_t arg5) {
    char* str = (char*)buffer;
    for (size_t i = 0; i < count && str[i] != '\0'; i++) {
        VGA_Putc(str[i]);
    }
    return count;
}

static uint64_t sys_getpid_handler(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    // TODO: برگرداندن PID واقعی
    return 1;
}

// سیستم‌کال‌های ساده دیگر
static uint64_t sys_dummy_handler(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    DEBUGER(LIGHT_RED, "Dummy syscall called");
    return 0;
}

void Init_Syscall(void) {
    // مقداردهی اولیه جدول سیستم‌کال
    for (int i = 0; i < SYSCALL_COUNT; i++) {
        syscall_table[i] = sys_dummy_handler;
    }

    // ثبت سیستم‌کال‌های پیاده‌سازی شده
    syscall_table[SYS_EXIT] = sys_exit_handler;
    syscall_table[SYS_WRITE] = sys_write_handler;
    syscall_table[SYS_GETPID] = sys_getpid_handler;

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
