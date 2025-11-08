#ifndef KERNEL_H
#define KERNEL_H

#define DEBUG 1

#define OS_NAME "M12 OS"
#define OS_VERSION "0.15"
#define OS kprintf("%s version %s Booted Successfully! \n", OS_NAME, OS_VERSION)

#ifdef DEBUG
#define DEBUGER(color, msg, ...) kprintcolor(color, "[DEBUG] " msg "\n", ##__VA_ARGS__)
#endif

#define SYSTEM_READY_MESSAGE "\nSystem ready for operation! \n"

#endif
