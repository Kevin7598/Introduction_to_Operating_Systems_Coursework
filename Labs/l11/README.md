# ECE4820J Lab 11

### Kaiqi Zhu 522370910091

## What is a file system

A filesystem is that piece of software that is in charge of storing, organizing and generally taking care of data represented as files and directories.

## How is the Linux VFS working

The Linux VFS (Virtual File System) is a unified interface that lets applications access many different file systems in the same way, acting like a translator and dispatcher between programs and actual file systems.

## What is FUSE, and how does it interact with the VFS? Can you sketch it quickly to make it clearer

FUSE (Filesystem in Userspace) is a software interface for Unix and Unix-like computer operating systems that lets non-privileged users create their own file systems without editing kernel code. can used to create file systems in user space, instead of writing kernel code. The Linux kernel still handles VFS. FUSE acts as a bridge between the kernel and a user-space program that implements filesystem logic.

```
Application
    |
Linux VFS
    |
FUSE kernel module
    |
FUSE userspace process
    |
Real storage / logic
```

## Explain what is SSHFS

SSHFS is a network file system based on SSH, which uses SSH for secure communication and SFTP under the hood.

## Use SSHFS to mount your home on Lemonion Inc. server in your computer

```
sshfs kevin7598@111.186.57.145:/home/kevin7598 ~
```

## What are the benefits of using SSHFS

SSHFS is a simple, secure way to work with remote files as if they were on the own machine, with the help of SSH and FUSE.
