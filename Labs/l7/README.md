# ECE4820J Lab 7

### Kaiqi Zhu 522370910091

## Fixing Dadfs

### 1

A kernel module is a piece of code that can be dynamically loaded into the Linux kernel at runtime without rebooting the system or recompiling the entire kernel, which extends kernel functionality.

Compared with a regular library, it executes in keneral space instead of user space, and has a higher privilege level. It extends kernel features, and is possible to have greater safety problems.

### 2

- Create the module source. Take `dadfs.c` as an example.
- Write a Makefile:

```makefile
obj-m += dadfs.o

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

- Compile: ```make```

### 3

`mutex` is defined as `DEFINE_MUTEX()` (3 kinds of definition in total). They are used with `mutex_lock_interruptible(&lock)` to acquire the lock and `mutex_unlock(&lock)` to release it. This approach works and follows standard kernel locking practices for synchronization in the kernel, but it is not optimal and can be improved as the mutexes are global, which can lead to contention and poor scalability on multi-core systems.

### 4

Information is shared between kernel and user space through the Virtual File System interface, which abstracts filesystem operations. User-space programs interact via standard system calls like `open(), read(), write(), mkdir(), readdir(), and stat()`, which invoke kernel callbacks defined in structures like `file_operations` and `inode_operations`. Data is copied between spaces using functions like `copy_to_user()`.
