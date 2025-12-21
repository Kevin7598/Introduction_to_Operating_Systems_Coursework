# ECE4820J Lab 9

### Kaiqi Zhu 522370910091

## Returned by `read` and `write`

For `read()`,
- $>$ 0: number of bytes successfully copied to user-space
- 0: EOF (end of file / nothing more to read)
- -EFAULT: invalid user pointer
- -EINVAL, -EPERM: other errors

For `write()`,
- $>$ 0: number of bytes actually consumed
- 0: usually means nothing accepted (rare)
- -EFAULT, -EINVAL: for errors

## How they are working

Major and minor numbers are shown in the fourth and fifth column below:

```shell
crw-r--r--  1 root  root     10, 235 Nov 26  2025 autofs
drwxr-xr-x  2 root  root         520 Nov 26 02:41 block
drwxr-xr-x  2 root  root          60 Nov 26  2025 bsg
crw-rw----  1 root  disk     10, 234 Nov 26  2025 btrfs-control
drwxr-xr-x  3 root  root          60 Nov 26  2025 bus
lrwxrwxrwx  1 root  root           3 Nov 26  2025 cdrom -> sr0
drwxr-xr-x  2 root  root        3900 Nov 26  2025 char
crw--w----  1 root  tty       5,   1 Nov 26  2025 console
lrwxrwxrwx  1 root  root          11 Nov 26  2025 core -> /proc/kcore
crw-------  1 root  root     10, 123 Nov 26  2025 cpu_dma_latency
crw-------  1 root  root     10, 203 Nov 26  2025 cuse
drwxr-xr-x  8 root  root         160 Nov 26 02:41 disk
brw-rw----  1 root  disk    252,   0 Nov 26  2025 dm-0
drwxr-xr-x  2 root  root          80 Jan  1  1970 dma_heap
drwxr-xr-x  3 root  root         100 Nov 26  2025 dri
crw-------  1 root  root     10, 125 Nov 26  2025 ecryptfs
crw-rw----  1 root  video    29,   0 Nov 26  2025 fb0
lrwxrwxrwx  1 root  root          13 Nov 26  2025 fd -> /proc/self/fd
crw-rw-rw-  1 root  root      1,   7 Nov 26  2025 full
crw-rw-rw-  1 root  root     10, 229 Nov 26 02:41 fuse
crw-------  1 root  root    235,   0 Nov 26  2025 hidraw0
crw-------  1 root  root    235,   1 Nov 26  2025 hidraw1
crw-------  1 root  root    235,   2 Nov 26  2025 hidraw2
drwxr-xr-x  2 root  root           0 Nov 26  2025 hugepages
crw-------  1 root  root     10, 183 Nov 26  2025 hwrng
lrwxrwxrwx  1 root  root          12 Nov 26  2025 initctl -> /run/initctl
drwxr-xr-x  4 root  root         380 Nov 26  2025 input
crw-r--r--  1 root  root      1,  11 Nov 26  2025 kmsg
lrwxrwxrwx  1 root  root          28 Nov 26  2025 log -> /run/systemd/journal/dev-log
crw-rw----  1 root  disk     10, 237 Nov 26 02:41 loop-control
brw-rw----  1 root  disk      7,   0 Nov 26  2025 loop0
brw-rw----  1 root  disk      7,   1 Nov 26  2025 loop1
brw-rw----  1 root  disk      7,  10 Nov 26  2025 loop10
brw-rw----  1 root  disk      7,  11 Nov 26  2025 loop11
brw-rw----  1 root  disk      7,  12 Nov 26  2025 loop12
brw-rw----  1 root  disk      7,  13 Nov 26  2025 loop13
brw-rw----  1 root  disk      7,  14 Nov 26  2025 loop14
brw-rw----  1 root  disk      7,  15 Nov 26  2025 loop15
brw-rw----  1 root  disk      7,  16 Nov 26  2025 loop16
brw-rw----  1 root  disk      7,  17 Nov 26 02:41 loop17
brw-rw----  1 root  disk      7,   2 Nov 26  2025 loop2
brw-rw----  1 root  disk      7,   3 Nov 26  2025 loop3
brw-rw----  1 root  disk      7,   4 Nov 26  2025 loop4
brw-rw----  1 root  disk      7,   5 Nov 26  2025 loop5
brw-rw----  1 root  disk      7,   6 Nov 26  2025 loop6
brw-rw----  1 root  disk      7,   7 Nov 26  2025 loop7
brw-rw----  1 root  disk      7,   8 Nov 26  2025 loop8
brw-rw----  1 root  disk      7,   9 Nov 26  2025 loop9
drwxr-xr-x  2 root  root          80 Nov 26  2025 mapper
crw-rw----+ 1 root  video   234,   0 Nov 26  2025 media0
crw-r-----  1 root  kmem      1,   1 Nov 26  2025 mem
drwxrwxrwt  2 root  root          40 Jan  1  1970 mqueue
drwxr-xr-x  2 root  root          60 Jan  1  1970 net
crw-------  1 root  root    236,   0 Nov 26  2025 ng0n1
crw-rw-rw-  1 root  root      1,   3 Nov 26  2025 null
crw-------  1 root  root    237,   0 Nov 26  2025 nvme0
brw-rw----  1 root  disk    259,   0 Nov 26  2025 nvme0n1
brw-rw----  1 root  disk    259,   1 Nov 26  2025 nvme0n1p1
brw-rw----  1 root  disk    259,   2 Nov 26  2025 nvme0n1p2
brw-rw----  1 root  disk    259,   3 Nov 26  2025 nvme0n1p3
crw-r-----  1 root  kmem      1,   4 Nov 26  2025 port
crw-------  1 root  root    108,   0 Nov 26  2025 ppp
crw-------  1 root  root     10,   1 Nov 26  2025 psaux
crw-rw-rw-  1 root  tty       5,   2 Nov 26 03:01 ptmx
crw-------  1 root  root    245,   0 Nov 26  2025 ptp0
drwxr-xr-x  2 root  root           0 Nov 26  2025 pts
crw-rw-rw-  1 root  root      1,   8 Nov 26  2025 random
crw-rw-r--+ 1 root  root     10, 242 Nov 26  2025 rfkill
lrwxrwxrwx  1 root  root           4 Nov 26  2025 rtc -> rtc0
crw-------  1 root  root    247,   0 Nov 26  2025 rtc0
crw-rw----+ 1 root  cdrom    21,   0 Nov 26  2025 sg0
drwxrwxrwt  2 root  root          40 Nov 26  2025 shm
drwxr-xr-x  3 root  root         180 Nov 26  2025 snd
brw-rw----+ 1 root  cdrom    11,   0 Nov 26  2025 sr0
lrwxrwxrwx  1 root  root          15 Nov 26  2025 stderr -> /proc/self/fd/2
lrwxrwxrwx  1 root  root          15 Nov 26  2025 stdin -> /proc/self/fd/0
lrwxrwxrwx  1 root  root          15 Nov 26  2025 stdout -> /proc/self/fd/1
crw-rw-rw-  1 root  tty       5,   0 Nov 26 02:48 tty
crw--w----  1 root  tty       4,   0 Nov 26  2025 tty0
crw--w----  1 root  tty       4,   1 Nov 26  2025 tty1
crw--w----  1 root  tty       4,  10 Nov 26  2025 tty10
crw--w----  1 root  tty       4,  11 Nov 26  2025 tty11
crw--w----  1 root  tty       4,  12 Nov 26  2025 tty12
crw--w----  1 root  tty       4,  13 Nov 26  2025 tty13
crw--w----  1 root  tty       4,  14 Nov 26  2025 tty14
crw--w----  1 root  tty       4,  15 Nov 26  2025 tty15
crw--w----  1 root  tty       4,  16 Nov 26  2025 tty16
crw--w----  1 root  tty       4,  17 Nov 26  2025 tty17
crw--w----  1 root  tty       4,  18 Nov 26  2025 tty18
crw--w----  1 root  tty       4,  19 Nov 26  2025 tty19
crw--w----  1 kevin tty       4,   2 Nov 26  2025 tty2
crw--w----  1 root  tty       4,  20 Nov 26  2025 tty20
crw--w----  1 root  tty       4,  21 Nov 26  2025 tty21
crw--w----  1 root  tty       4,  22 Nov 26  2025 tty22
crw--w----  1 root  tty       4,  23 Nov 26  2025 tty23
crw--w----  1 root  tty       4,  24 Nov 26  2025 tty24
crw--w----  1 root  tty       4,  25 Nov 26  2025 tty25
crw--w----  1 root  tty       4,  26 Nov 26  2025 tty26
crw--w----  1 root  tty       4,  27 Nov 26  2025 tty27
crw--w----  1 root  tty       4,  28 Nov 26  2025 tty28
crw--w----  1 root  tty       4,  29 Nov 26  2025 tty29
crw--w----  1 root  tty       4,   3 Nov 26  2025 tty3
crw--w----  1 root  tty       4,  30 Nov 26  2025 tty30
crw--w----  1 root  tty       4,  31 Nov 26  2025 tty31
crw--w----  1 root  tty       4,  32 Nov 26  2025 tty32
crw--w----  1 root  tty       4,  33 Nov 26  2025 tty33
crw--w----  1 root  tty       4,  34 Nov 26  2025 tty34
crw--w----  1 root  tty       4,  35 Nov 26  2025 tty35
crw--w----  1 root  tty       4,  36 Nov 26  2025 tty36
crw--w----  1 root  tty       4,  37 Nov 26  2025 tty37
crw--w----  1 root  tty       4,  38 Nov 26  2025 tty38
crw--w----  1 root  tty       4,  39 Nov 26  2025 tty39
crw--w----  1 root  tty       4,   4 Nov 26  2025 tty4
crw--w----  1 root  tty       4,  40 Nov 26  2025 tty40
crw--w----  1 root  tty       4,  41 Nov 26  2025 tty41
crw--w----  1 root  tty       4,  42 Nov 26  2025 tty42
crw--w----  1 root  tty       4,  43 Nov 26  2025 tty43
crw--w----  1 root  tty       4,  44 Nov 26  2025 tty44
crw--w----  1 root  tty       4,  45 Nov 26  2025 tty45
crw--w----  1 root  tty       4,  46 Nov 26  2025 tty46
crw--w----  1 root  tty       4,  47 Nov 26  2025 tty47
crw--w----  1 root  tty       4,  48 Nov 26  2025 tty48
crw--w----  1 root  tty       4,  49 Nov 26  2025 tty49
crw--w----  1 root  tty       4,   5 Nov 26  2025 tty5
crw--w----  1 root  tty       4,  50 Nov 26  2025 tty50
crw--w----  1 root  tty       4,  51 Nov 26  2025 tty51
crw--w----  1 root  tty       4,  52 Nov 26  2025 tty52
crw--w----  1 root  tty       4,  53 Nov 26  2025 tty53
crw--w----  1 root  tty       4,  54 Nov 26  2025 tty54
crw--w----  1 root  tty       4,  55 Nov 26  2025 tty55
crw--w----  1 root  tty       4,  56 Nov 26  2025 tty56
crw--w----  1 root  tty       4,  57 Nov 26  2025 tty57
crw--w----  1 root  tty       4,  58 Nov 26  2025 tty58
crw--w----  1 root  tty       4,  59 Nov 26  2025 tty59
crw--w----  1 root  tty       4,   6 Nov 26  2025 tty6
crw--w----  1 root  tty       4,  60 Nov 26  2025 tty60
crw--w----  1 root  tty       4,  61 Nov 26  2025 tty61
crw--w----  1 root  tty       4,  62 Nov 26  2025 tty62
crw--w----  1 root  tty       4,  63 Nov 26  2025 tty63
crw--w----  1 root  tty       4,   7 Nov 26  2025 tty7
crw--w----  1 root  tty       4,   8 Nov 26  2025 tty8
crw--w----  1 root  tty       4,   9 Nov 26  2025 tty9
crw-rw----  1 root  dialout   4,  64 Nov 26  2025 ttyS0
crw-rw----  1 root  dialout   4,  65 Nov 26  2025 ttyS1
crw-rw----  1 root  dialout   4,  74 Nov 26  2025 ttyS10
crw-rw----  1 root  dialout   4,  75 Nov 26  2025 ttyS11
crw-rw----  1 root  dialout   4,  76 Nov 26  2025 ttyS12
crw-rw----  1 root  dialout   4,  77 Nov 26  2025 ttyS13
crw-rw----  1 root  dialout   4,  78 Nov 26  2025 ttyS14
crw-rw----  1 root  dialout   4,  79 Nov 26  2025 ttyS15
crw-rw----  1 root  dialout   4,  80 Nov 26  2025 ttyS16
crw-rw----  1 root  dialout   4,  81 Nov 26  2025 ttyS17
crw-rw----  1 root  dialout   4,  82 Nov 26  2025 ttyS18
crw-rw----  1 root  dialout   4,  83 Nov 26  2025 ttyS19
crw-rw----  1 root  dialout   4,  66 Nov 26  2025 ttyS2
crw-rw----  1 root  dialout   4,  84 Nov 26  2025 ttyS20
crw-rw----  1 root  dialout   4,  85 Nov 26  2025 ttyS21
crw-rw----  1 root  dialout   4,  86 Nov 26  2025 ttyS22
crw-rw----  1 root  dialout   4,  87 Nov 26  2025 ttyS23
crw-rw----  1 root  dialout   4,  88 Nov 26  2025 ttyS24
crw-rw----  1 root  dialout   4,  89 Nov 26  2025 ttyS25
crw-rw----  1 root  dialout   4,  90 Nov 26  2025 ttyS26
crw-rw----  1 root  dialout   4,  91 Nov 26  2025 ttyS27
crw-rw----  1 root  dialout   4,  92 Nov 26  2025 ttyS28
crw-rw----  1 root  dialout   4,  93 Nov 26  2025 ttyS29
crw-rw----  1 root  dialout   4,  67 Nov 26  2025 ttyS3
crw-rw----  1 root  dialout   4,  94 Nov 26  2025 ttyS30
crw-rw----  1 root  dialout   4,  95 Nov 26  2025 ttyS31
crw-rw----  1 root  dialout   4,  68 Nov 26  2025 ttyS4
crw-rw----  1 root  dialout   4,  69 Nov 26  2025 ttyS5
crw-rw----  1 root  dialout   4,  70 Nov 26  2025 ttyS6
crw-rw----  1 root  dialout   4,  71 Nov 26  2025 ttyS7
crw-rw----  1 root  dialout   4,  72 Nov 26  2025 ttyS8
crw-rw----  1 root  dialout   4,  73 Nov 26  2025 ttyS9
crw-------  1 root  root      5,   3 Nov 26  2025 ttyprintk
drwxr-xr-x  2 root  root          60 Nov 26  2025 ubuntu-vg
crw-rw----  1 root  kvm      10, 124 Nov 26  2025 udmabuf
crw-------  1 root  root     10, 239 Nov 26  2025 uhid
crw-------  1 root  root     10, 223 Nov 26  2025 uinput
crw-rw-rw-  1 root  root      1,   9 Nov 26  2025 urandom
crw-------  1 root  root     10, 126 Nov 26  2025 userfaultfd
crw-------  1 root  root     10, 240 Nov 26  2025 userio
drwxr-xr-x  4 root  root          80 Nov 26  2025 v4l
crw-rw----  1 root  tty       7,   0 Nov 26  2025 vcs
crw-rw----  1 root  tty       7,   1 Nov 26  2025 vcs1
crw-rw----  1 root  tty       7,   2 Nov 26  2025 vcs2
crw-rw----  1 root  tty       7,   3 Nov 26  2025 vcs3
crw-rw----  1 root  tty       7,   4 Nov 26  2025 vcs4
crw-rw----  1 root  tty       7,   5 Nov 26  2025 vcs5
crw-rw----  1 root  tty       7,   6 Nov 26  2025 vcs6
crw-rw----  1 root  tty       7, 128 Nov 26  2025 vcsa
crw-rw----  1 root  tty       7, 129 Nov 26  2025 vcsa1
crw-rw----  1 root  tty       7, 130 Nov 26  2025 vcsa2
crw-rw----  1 root  tty       7, 131 Nov 26  2025 vcsa3
crw-rw----  1 root  tty       7, 132 Nov 26  2025 vcsa4
crw-rw----  1 root  tty       7, 133 Nov 26  2025 vcsa5
crw-rw----  1 root  tty       7, 134 Nov 26  2025 vcsa6
crw-rw----  1 root  tty       7,  64 Nov 26  2025 vcsu
crw-rw----  1 root  tty       7,  65 Nov 26  2025 vcsu1
crw-rw----  1 root  tty       7,  66 Nov 26  2025 vcsu2
crw-rw----  1 root  tty       7,  67 Nov 26  2025 vcsu3
crw-rw----  1 root  tty       7,  68 Nov 26  2025 vcsu4
crw-rw----  1 root  tty       7,  69 Nov 26  2025 vcsu5
crw-rw----  1 root  tty       7,  70 Nov 26  2025 vcsu6
drwxr-xr-x  2 root  root          60 Nov 26  2025 vfio
crw-------  1 root  root     10, 127 Nov 26  2025 vga_arbiter
crw-------  1 root  root     10, 137 Nov 26  2025 vhci
crw-rw----  1 root  kvm      10, 238 Nov 26 02:41 vhost-net
crw-rw----  1 root  kvm      10, 241 Nov 26 02:41 vhost-vsock
crw-rw----+ 1 root  video    81,   0 Nov 26  2025 video0
crw-rw----+ 1 root  video    81,   1 Nov 26  2025 video1
crw-------  1 root  root     10, 122 Nov 26  2025 vmci
crw-rw-rw-  1 root  root     10, 121 Nov 26  2025 vsock
crw-rw-rw-  1 root  root      1,   5 Nov 26  2025 zero
```

The major number identifies the device driver responsible for controlling a particular type of device, while the minor number identifies a specific instance of a device within a class of devices controlled by the same driver.

## How to add a character device to /dev

```shell
sudo mknod /dev/dice c 240 0
```

## Locations of the terms

| Term                  | What it is                              | Source location                 |
|:-----------------------:|----------------------------------------|--------------------------------|
| module_init            | macro to register module initialization | include/linux/module.h         |
| module_exit            | macro for cleanup function              | include/linux/module.h         |
| MAJOR                  | extract major number from dev_t         | include/linux/kdev_t.h         |
| MINOR                  | extract minor number                     | include/linux/kdev_t.h         |
| MKDEV                  | create dev_t from major/minor           | include/linux/kdev_t.h         |
| dev_t                  | type representing device numbers        | include/linux/kdev_t.h         |
| cdev_init              | initialize struct cdev                  | fs/char_dev.c                  |
| cdev_add               | add char device to system               | fs/char_dev.c                  |
| cdev_del               | remove char device                       | fs/char_dev.c                  |
| alloc_chrdev_region    | dynamically allocate major/minor        | fs/char_dev.c                  |
| printk                 | kernel printf                           | kernel/printk/*                |
| container_of           | extract struct pointer from its member | include/linux/kernel.h         |
| module_param           | define module options                   | include/linux/moduleparam.h    |
| THIS_MODULE            | pointer to current module               | include/linux/module.h         |

## How to generate random numbers when working inside the Linux kernel

In kernel sapce, use functions like:

```c
#include <linux/random.h>

u32 x = get_random_u32();
u64 y = get_random_u64();
```

## How to define and specify module options

Use

```c
module_param(name, type, permissions)
```
