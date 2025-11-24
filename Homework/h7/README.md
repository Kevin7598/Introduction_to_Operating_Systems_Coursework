# ECE4820J Homework 7

### Kaiqi Zhu 522370910091

## Ex. 1

### 1

The new table:

| Page | Time stamp | Present | Referenced | Modified |
|:-:|:-:|:-:|:-:|:-:|
| 0 | 6 | 1 | 0 | 1 |
| 1 | 9 | 1 | 0 | 0 |
| 2 | 9 | 1 | 0 | 1 |
| 3 | 7 | 1 | 0 | 0 |
| 4 | 4 | 0 | 0 | 0 |

### 2

The new table:

| Page | Time stamp | Present | Referenced | Modified |
|:-:|:-:|:-:|:-:|:-:|
| 0 | 6 | 1 | 0 | 1 |
| 1 | 9 | 1 | 0 | 0 |
| 2 | 9 | 1 | 0 | 1 |
| 3 | 10 | 1 | 0 | 0 |
| 4 | 4 | 0 | 0 | 0 |

## Ex. 2

### 1

The system call: `do_fork()`

### 2

Program (in Python):

```python
from bcc import BPF

bpf_text = r"""
BPF_HASH(fork_count, u32, u64);

int count_fork(struct pt_regs *ctx) {
    u32 key = 0;
    u64 zero = 0, *val;

    val = fork_count.lookup_or_init(&key, &zero);
    (*val)++;
    return 0;
}
"""

b = BPF(text=bpf_text)

b.attach_kprobe(event="do_fork", fn_name="count_fork")

print("Tracing process forks... Ctrl-C to stop.")

try:
    while True:
        pass
except KeyboardInterrupt:
    print("\nFork count:")
    key = 0
    value = b["fork_count"][key]
    print(value.value)
```

### 3

`fentry` is a tool that can trace kernel functions in eBPF in a modern way. Compared with kprobes, it is newer and more efficient, and has a much lower overhead than kprobes. It can be used with

```python
b.attach_fentry("do_fork", fn_name="count_fork")
```

to provide lower overhead, faster dispatch, more stable attachment

## Ex, 3

Ext2, or second extended file system, is a file system for the Linux kernel. It was initially designed by French software developer Remy Card as a replacement for the extended file system (ext). Having been designed according to the same principles as the Berkeley Fast File System from BSD, it was the first commercial-grade filesystem for Linux.

The space in ext2 is split up into blocks. These blocks are grouped into block groups, analogous to cylinder groups in the Unix File System. There are typically thousands of blocks on a large file system. Data for any given file is typically contained within a single block group where possible. This is done to minimize the number of disk seeks when reading large amounts of contiguous data.

Each block group contains a copy of the superblock and block group descriptor table, and all block groups contain a block bitmap, an inode bitmap, an inode table, and finally the actual data blocks.

The superblock contains important information that is crucial to the booting of the operating system. Thus backup copies are made in multiple block groups in the file system. However, typically only the first copy of it, which is found at the first block of the file system, is used in the booting.

The group descriptor stores the location of the block bitmap, inode bitmap, and the start of the inode table for every block group. These, in turn, are stored in a group descriptor table.

Each directory is a list of directory entries. Each directory entry associates one file name with one inode number, and consists of the inode number, the length of the file name, and the actual text of the file name. To find a file, the directory is searched front-to-back for the associated filename. For reasonable directory sizes, this is fine. But for very large directories this is inefficient, and ext3 offers a second way of storing directories (HTree) that is more efficient than just a list of filenames.

The root directory is always stored in inode number two, so that the file system code can find it at mount time. Subdirectories are implemented by storing the name of the subdirectory in the name field, and the inode number of the subdirectory in the inode field. Hard links are implemented by storing the same inode number with more than one file name. Accessing the file by either name results in the same inode number, and therefore the same data.

When a new file or directory is created, ext2 must decide where to store the data. If the disk is mostly empty, then data can be stored almost anywhere. However, clustering the data with related data will minimize seek times and maximize performance.

ext2 attempts to allocate each new directory in the group containing its parent directory, on the theory that accesses to parent and children directories are likely to be closely related. ext2 also attempts to place files in the same group as their directory entries, because directory accesses often lead to file accesses. However, if the group is full, then the new file or new directory is placed in some other non-full group.

The data blocks needed to store directories and files can be found by looking in the data allocation bitmap. Any needed space in the inode table can be found by looking in the inode allocation bitmap.

(Source: [Ext2](https://en.wikipedia.org/wiki/Ext2))

## Ex. 4

### 1

No, because the read and write permission bit is stored in the page table entry, and a PTE describes protections for a physical frame. Therefore if two processes share the same physical page frame, they must map that page using the same protection level.

### 2

No, because a TLB entry already includes segment base and bounds, page number inside the segment, physical frame number, and protection bits. Therefore, no memory access is required to the table, and segment table lookup and page table lookup can be done together.
