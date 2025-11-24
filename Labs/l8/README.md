# ECE4820J Lab 8

### Kaiqi Zhu 522370910091

## `kmsan`, `kasan`, and `kfence` folders

`kmsan` is Kernel Memory Sanitizer. `kasan` is Kernel Address Sanitizer. `kfence` is Kernel Electric Fence.

## Shadow Memory

Shadow memory is the extra memory the sanitizer uses to track metadata about each byte of real memory. It is the key mechanism that allows sanitizers to detect memory bugs at runtime.

## Sanitizers

Sanitizers are runtime instrumentation tools used to detect memory and concurrency bugs. They are essential because kernel code has no memory safety, runs with full privileges, and has no user-level protections. Therefore, They are basic safety guarantee for kernel development.

## `kmemleak.c`

`kmemleak.c` is the built-in memory leak detector of the Lunix kernel, which should be used by kernel developers, when developing new subsystems or when hunting long-term memory leaks.

## `nommu.c`

`nommu.c` can be used in very small embedded MCUs, DSPs, and older embedded SoCs.

Drawbacks of having CPU without any MMU:
- No memory isolation: one bug crashes whole system.
- No process separation: cannot run multiple user programs securely.
- No paging: cannot run programs larger than the size of RAM.
- No guard pages: making stack/heap overflow detection nearly impossible.

## `compaction.c`

`compaction.c` is used for reducing physical memory fragmentation, and creating contiguous free physical page ranges. "memory compaction" is important because it avoids high-order allocation failures, reduces direct reclaim pressure, helps with huge page allocation.

## OOM Killer

The OOM killer is a last-resort mechanism when the system is critically low on memory and cannot satisfy an allocation request. In particular, OOM means Out Of Memory.

## Reason for a deadlock in the Linux kernel

Deadlock can occur when memory allocation happens inside a critical section, while the system is low on memory, and memory reclaim needs that same lock.

Example Scenario:
- Process A holds a lock.
- Process A tries to allocate memory.
- Memory is exhausted.
- Page reclaim runs.
- Page reclaim cannot proceed.
  - It needs to lock resources that process A holds.
  - The only reclaimable memory belongs to processes waiting on A.

## The rough layout of kernel and user space memories

The layout of kernel space memory is shown in [here](https://stackoverflow.com/questions/60076669/kernel-virtual-memory-space-and-process-virtual-memory-space):

![kernel](./img/kernel_space.jpg)

The layout of user space memory is shown in [here](https://stackoverflow.com/questions/22801666/memory-layout-of-stack-and-heap-in-user-space):

![space](./img/user_space.jpg)

## Whether or not kernel memory can be swapped and why

No, kernel memory cannot be swapped, because kernel must always be resident, kernel cannot page-fault on itself, kernel pointers may become invalid if swapped.
