#ifndef SYSCALL_H
#define SYSCALL_H

#include "type.h"

#define SYSCALL_COUNT 10

// شماره سیستم‌کال‌ها
#define SYS_EXIT     0
#define SYS_READ     1
#define SYS_WRITE    2
#define SYS_OPEN     3
#define SYS_CLOSE    4
#define SYS_FORK     5
#define SYS_EXEC     6
#define SYS_WAIT     7
#define SYS_GETPID   8
#define SYS_BRK      9

// ساختار پارامترهای سیستم‌کال
typedef struct {
    uint64_t arg1, arg2, arg3, arg4, arg5;
} SyscallArgs;

// توابع
void Init_Syscall(void);
uint64_t syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

// اعلام syscall_entry از اسمبلی
extern void syscall_entry(void);

// توابع سیستم‌کال
void sys_exit(int status);
int sys_read(int fd, void* buffer, size_t count);
int sys_write(int fd, const void* buffer, size_t count);
int sys_open(const char* path, int flags);
int sys_close(int fd);
int sys_fork(void);
int sys_exec(const char* path, char* const argv[]);
int sys_wait(int* status);
int sys_getpid(void);
void* sys_brk(void* addr);

#endif