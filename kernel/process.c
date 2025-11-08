#include "../include/process.h"
#include "../include/memory.h"
#include "../include/paging.h"
#include "../include/common.h"

static process_t processes[MAX_PROCESSES];
static process_t* current_process = NULL;
static uint32_t next_pid = 1;
static uint32_t process_count = 0;

// پردازش idle (وقتی هیچ پردازشی آماده نباشد)
static void idle_process(void) {
    while (1) {
        asm volatile("hlt");
    }
}

void InitProcessManager(void) {
    //DEBUGER(LIGHT_CYAN, "Initializing process manager...");
    
    // مقداردهی اولیه متغیرهای global
    current_process = NULL;
    next_pid = 1;
    process_count = 0;
    
    // پاک کردن تمام پردازش‌ها
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROCESS_UNUSED;
        processes[i].pid = 0;
        processes[i].parent = NULL;
        processes[i].kernel_stack = 0;
        processes[i].user_stack = 0;
        processes[i].page_table = NULL;
    }
    
    // ایجاد پردازش idle
    process_t* idle = create_process(idle_process);
    if (idle) {
        idle->pid = 1;  // تغییر: PID از 1 شروع شود، نه 0
        current_process = idle;
      //  DEBUGER(LIGHT_GREEN, "Idle process created with PID %d", idle->pid);
    } else {
      //  DEBUGER(LIGHT_RED, "Failed to create idle process! Creating fallback...");
        
        // ایجاد fallback manual
        current_process = &processes[0];
        current_process->pid = 1;
        current_process->state = PROCESS_RUNNING;
        current_process->page_table = get_kernel_page_table();
        current_process->brk = 0x400000;
        process_count = 1;
        
     //   DEBUGER(LIGHT_YELLOW, "Fallback process created with PID %d", current_process->pid);
    }
    
    DEBUGER(LIGHT_RED, "Process manager initialized with %d processes", process_count);
}

process_t* create_process(void (*entry_point)(void)) {
    // یافتن slot خالی
    int free_slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_UNUSED) {
            free_slot = i;
            break;
        }
    }
    
    if (free_slot == -1) {
        DEBUGER(LIGHT_RED, "No free process slots available");
        return NULL;
    }
    
    process_t* proc = &processes[free_slot];
    
    // مقداردهی اولیه پردازش
    proc->pid = next_pid++;
    proc->state = PROCESS_READY;
    
    // استفاده از صفحه‌بندی کرنل اگر موجود است
    void* kernel_pt = get_kernel_page_table();
    proc->page_table = kernel_pt ? kernel_pt : NULL;
    
    proc->parent = current_process;
    proc->brk = 0x400000;
    
    // تخصیص پشته کرنل
    proc->kernel_stack = (uint64_t)get_physical_page();
    if (!proc->kernel_stack) {
        DEBUGER(LIGHT_RED, "Failed to allocate kernel stack");
        return NULL;
    }
    
    // تخصیص پشته کاربر
    proc->user_stack = (uint64_t)get_physical_page();
    if (!proc->user_stack) {
        DEBUGER(LIGHT_RED, "Failed to allocate user stack");
        free_physical_page((void*)proc->kernel_stack);
        return NULL;
    }
    
    // راه‌اندازی زمینه پردازش
    memset(&proc->context, 0, sizeof(process_context_t));
    
    // تنظیم instruction pointer
    proc->context.rip = (uint64_t)entry_point;
    
    // تنظیم segment registers
    proc->context.cs = 0x08;
    proc->context.ss = 0x10;
    
    // تنظیم stack pointer
    proc->context.rsp = proc->kernel_stack + KERNEL_STACK_SIZE;
    
    // تنظیم flags (فعال کردن وقفه)
    proc->context.rflags = 0x202;
    
    process_count++;
    
    //DEBUGER(LIGHT_GREEN, "Created process %d: entry=0x%x, kstack=0x%x, ustack=0x%x", 
      //     proc->pid, entry_point, proc->kernel_stack, proc->user_stack);
    
    return proc;
}

void schedule(void) {
    if (process_count <= 1) {
        return;
    }
    
    // الگوریتم زمان‌بندی ساده Round Robin
    uint32_t current_index = current_process - processes;
    uint32_t next_index = (current_index + 1) % MAX_PROCESSES;
    
    // یافتن پردازش بعدی آماده
    for (uint32_t i = 0; i < MAX_PROCESSES; i++) {
        process_t* next = &processes[next_index];
        
        if (next->state == PROCESS_READY && next != current_process) {
            process_t* old = current_process;
            current_process = next;
            
            DEBUGER(LIGHT_MAGENTA, "Context switch: %d -> %d", old->pid, next->pid);
            
            // فراخوانی تابع اسمبلی برای تعویض زمینه
            switch_process(&old->context, &next->context);
            return;
        }
        
        next_index = (next_index + 1) % MAX_PROCESSES;
    }
}

uint32_t get_current_pid(void) {
    if (current_process == NULL) {
        DEBUGER(LIGHT_RED, "CRITICAL: current_process is NULL!");
        return 0;
    }
    
    if (current_process->pid == 0) {
        DEBUGER(LIGHT_RED, "CRITICAL: current_process->pid is 0!");
        return 0;
    }
    
    return current_process->pid;
}

process_t* get_current_process(void) {
    return current_process;
}

// ==================== سیستم‌کال‌ها ====================

uint64_t sys_fork_handler(void) {
    DEBUGER(LIGHT_CYAN, "sys_fork called from process %d", get_current_pid());
    
    // ایجاد پردازش فرزند
    process_t* child = create_process((void(*)())current_process->context.rip);
    if (!child) {
        return -1;
    }
    
    // کپی کردن زمینه از والد
    child->context = current_process->context;
    
    // تنظیم مقدار بازگشت برای فرزند (rax = 0)
    child->context.rax = 0;
    
    return child->pid;
}

uint64_t sys_exec_handler(const char* path, char* const argv[]) {
    DEBUGER(LIGHT_CYAN, "sys_exec called with path: %s", path);
    return -1;
}

uint64_t sys_exit_handler_real(uint32_t status) {
    uint32_t pid = get_current_pid();
    DEBUGER(LIGHT_CYAN, "Process %d exiting with status %d", pid, status);
    
    if (current_process) {
        current_process->state = PROCESS_ZOMBIE;
        current_process->exit_code = status;
        process_count--;
    }
    
    schedule();
    return 0;
}

uint64_t sys_wait_handler(int* status) {
    DEBUGER(LIGHT_CYAN, "sys_wait called from process %d", get_current_pid());
    return -1;
}

uint64_t sys_getpid_handler_real(void) {
    uint32_t pid = get_current_pid();
    
    // دیباگ
    if (pid == 0) {
        DEBUGER(LIGHT_RED, "WARNING: get_current_pid() returned 0");
        DEBUGER(LIGHT_RED, "current_process = 0x%x", current_process);
        if (current_process) {
            DEBUGER(LIGHT_RED, "current_process->pid = %d", current_process->pid);
        }
    }
    
    return pid;
}

uint64_t sys_brk_handler(void* addr) {
    if (current_process) {
        if (addr != NULL) {
            current_process->brk = (uint64_t)addr;
        }
        return current_process->brk;
    }
    return 0;
}
