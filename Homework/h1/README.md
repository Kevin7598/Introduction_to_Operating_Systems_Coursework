# ECE4820J Homework 1
Kaiqi Zhu 522370910091

## Ex. 1
| Feature | Stack | Heap |
|:-------:|----------|----------|
| Definition | A region of memory that stores temporary data for functions | A large pool of memory available for programs to use for data that needs to live for a longer time, beyond a single function call |
| Management | Highly organized and managed automatically by the CPU. It operates on a LIFO principle. When a function is called, a "frame" containing its local data is pushed onto the stack. When the function finishes, its frame is popped off, and the memory is automatically freed. | Not managed automatically. The programmer is responsible for explicitly allocating memory and deallocating it when it's no longer needed.
| Allocation |Static, at compile time|Dyanamic, at run time|
| Speed | Fast | Slow |
| Size | Fixed | Flexible |
| Main Use | Function calls, local variables | Objects, data with a long lifespan |
| Key Risk | Stack Overflow | Memory Leaks & Fragmentation |

## Ex. 2
#### 1. 

- When the computer is powered on, the CPU will start and begin executing firmware code stored on a chip on the motherboard, called the BIOS. Then the BIOS will run a check to ensure essential hardware is present and working correctly and consult a pre-configured list of storage devices to find a bootable one, which loads the first sector from the bootable device containing a small program called a bootloader. The bootloader's job is to find and load the main part of the operating system, the kernel, from the drive into RAM. The bootloader transfers control to the kernel. The OS then initializes the rest of the system: it starts device drivers, system processes, and finally, the user interface. 

- The role of the BIOS is to be the initial bridge between the hardware and the software, which initializes the hardware and provides the bootloader with the necessary services to start loading the actual operating system.

#### 2. 

**Hybrid kernel** is a combination of monolithic and microkernel design, with core services running in kernel space for speed, but some components may run in user space for modularity, while **exokernel** is a minimal kernel that exposes hardware resources directly to applications, letting them manage resources themselves.

## Ex. 3
#### 1. 

a), c), and d)

a) If a user program could disable interrupts, it could prevent the OS from multitasking or responding to hardware, effectively freezing the entire system.

b) Reading the time is a "read-only" operation that doesn't interfere with other processes.

c) Changing the system time could disrupt critical system functions including file timestamps and scheduled tasks.

d) The memory map controls how memory is allocated. If a user program could change it, it could access the memory of other programs or even the kernel itself, leading to a total collapse of security and stability.

#### 2.

20ms.

As there are 2 * 2 = 4 threads in total, the three programs can be run simultaneously. Therefore, it will take 20ms to execute the programs.

## Ex. 4

``` bash
# Create a new user.
sudo adduser kaiqi
# List all the currently running processes.
ps aux
# Display the characteristics of the CPU.
lscpu
# Display the characteristics of the available memory.
free -h
# Redirect random output read from a system device, into two different files.
dd if=/dev/urandom bs=1k count=1 2>/dev/null | tee file1 file2 >/dev/null
# Concatenate file 1 and file 2.
cat file1 file2 > combined_file
# Read the file as hexadecimal values.
hexdump -C combined_file
# Find specific files.
find . -type f -name "*mutex*" -exec grep -lw "nest_lock" {} \;
```
