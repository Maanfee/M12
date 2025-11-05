# M12 OS - A Modern x86-64 Operating System

![Architecture](https://img.shields.io/badge/Architecture-x86__64-green.svg)
![Kernel](https://img.shields.io/badge/Kernel-Microkernel%20(Developing)-orange.svg)

## 📖 Overview

**M12** is a modern, from-scratch x86-64 operating system written in C. The project follows a microkernel architecture and demonstrates low-level systems programming concepts including bootloading, memory management, and hardware interaction.

<!--
## 🏗️ Project Structure
src/
|
├── include/
│ ├── kernel/
│ │ ├── asm.h # Inline assembly wrappers and hardware I/O
│ │ ├── common.h # Common function declarations
│ │ ├── kernel.h # Kernel version and constants
│ │ ├── type.h # Standard type definitions
│ │ └── vga.h # VGA text mode driver interface
│ ├── drivers/
│ │ ├── keyboard.h # [Planned] Keyboard driver
│ │ ├── timer.h # [Planned] Timer driver
│ │ └── serial.h # [Planned] Serial port driver
│ ├── services/
│ │ ├── filesystem.h # [Planned] Filesystem interface
│ │ └── network.h # [Planned] Network stack
| |
├── kernel/
│ ├── memory/
│ │ ├── paging.c # [Planned] Virtual memory management
│ │ └── heap.c # [Planned] Dynamic memory allocation
│ ├── process/
│ │ ├── scheduler.c # [Planned] Process scheduling
│ │ └── pcb.c # [Planned] Process control blocks
│ ├── interrupts/
│ │ ├── idt.c # [Planned] Interrupt Descriptor Table
│ │ └── isr.asm # [Planned] Interrupt Service Routines
│ └── ipc/
│ ├── message.c # [Planned] Inter-process communication
│ └── ports.c # [Planned] Message passing
├── drivers/
│ ├── keyboard.c # [Planned] Keyboard driver implementation
│ ├── timer.c # [Planned] Timer driver implementation
│ └── serial.c # [Planned] Serial driver implementation
└── services/
├── filesystem.c # [Planned] Filesystem implementation
└── network.c # [Planned] Network stack implementation
-->

## 🎯 Architecture Vision
<!--
┌─────────────────┐
│   User Space    │ ← Processes & Services
├─────────────────┤
│  System Calls   │ ← POSIX Interface
├─────────────────┤
│  Microkernel    │ ← IPC, Scheduling, MM
└─────────────────┘
-->

### 🏗️ Microkernel Design (In Development)
**M12** is being developed as a **microkernel** with the following design principles:

- **Minimal Kernel**: Kernel only handles essential functions:
  - Process scheduling and IPC
  - Memory management
  - Interrupt handling
- **User-space Services**: Drivers and services run as separate processes
- **Message Passing**: Inter-process communication via message queues
- **Modularity**: Each component can be developed and tested independently

### 📋 Planned Microkernel Components

#### Core Kernel
- **Process Manager**: Process creation, termination, scheduling
- **Memory Manager**: Virtual memory, paging, heap allocation
- **IPC System**: Message passing between processes
- **Interrupt Dispatcher**: Hardware interrupt routing

#### User-space Services
- **Device Drivers**: Filesystem, network, input devices
- **System Services**: Windowing system, authentication
- **Application Support**: Library OS for application compatibility

## 🛠️ Building and Running

### Prerequisites
- NASM (Netwide Assembler)
- GCC Cross-Compiler (x86_64-elf)
- GNU Make
- QEMU (for emulation)

### Installation Setup
```bash
# Update Ubuntu/Debian
sudo apt update && sudo apt upgrade -y
```
```bash
# Install build dependencies Ubuntu/Debian
sudo apt-get install -y build-essential
sudo apt-get install -y gcc-x86-64-linux-gnu
sudo apt-get install -y nasm
sudo apt-get install -y binutils
sudo apt-get install -y make
sudo apt-get install -y qemu-system-x86
sudo apt-get install -y gdb
```
### Symbolic Link (For WSL Users)
Create symbolic link for WSL (adjust path as needed)
```bash
# Symbolic Link Ubuntu/Debian
ln -s /mnt/<Windows Path>/M12 ~/m12
```
<!--
ln -s /mnt/c/Users/gordon/Desktop/Projects/M12 ~/m12
-->

### Build Instructions
```bash
# Build the OS image and Run
make

# Run in QEMU
make run

# Debug with QEMU and GDB
make debug

# Clean all generated files including OS image
make clean
