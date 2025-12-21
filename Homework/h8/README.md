# ECE4820J Homework 8

### Kaiqi Zhu 522370910091

## Ex. 1

### 1

No, because the wireless network is usually much slower than the scanner's internal data throughput.

### 2

With using a modular device driver model, drivers are stored as loadable kernel modules. The OS provides dynamic driver loading, so drivers can be inserted or removed on-the-fly. A device manager detects new hardware automatically and loads the correct driver. Standardized interfaces allow new devices to plug into existing OS frameworks. Therefore, recompiling the kernel or rebooting the system is not needed.

### 3

a) Device drivers
b) Device drivers
c) Device-independent OS software
d) User-level IO software

### 4

$Speed = 4 \times \frac{10^{9}}{100\times2} \times 32 \div 8 = 80 MB/s$

### 5

A thin client is a lightweight computer that relies on a remote server for computation, storage, and most applications. It features minimal local processing power and local storage, can handle most tasks on the server, and mainly handles input/output.

### 6

A hard disk stores data on magnetic rotating platters. It includes tracks (concentric circles on each platter), sectors (subdivisions of tracks), cylinders (set of tracks aligned vertically across platters), heads (one read/write head per platter surface).

### 7

**RAID 0** focuses purely on speed and capacity, completely ignoring safety. It takes your data and splits it evenly across two or more drives. **RAID 1** focuses on safety. It creates an exact copy of your data on two or more drives. **RAID 5** attempts to strike a balance between speed, safety, and storage space. It requires a minimum of 3 drives. **RAID 6** is essentially RAID 5 with an extra safety net. It requires a minimum of 4 drives. **RAID 10** is a "nested" RAID. It combines RAID 1 and RAID 0 to get the best features of both. It requires a minimum of 4 drives.

## Ex. 2

### 1

`SWP` can be used to implement a lcok for synchronization.

```asm
LOCK:   MOV  R, #1
TRY:    SWP  R, lock
        CMP  R, #0
        BNE  TRY
UNLOCK: MOV lock, #0
```

### 2

Virtualization is a technique where a single physical machine is abstracted into multiple logical virtual machines, each appearing to run on its own hardware. Main challenges include trapping privileged instructions, memory virtualization, I/O virtualization, performance overhead, scheduling fairness, and ensuring security isolation.

## Ex. 3

### 1

`/usr/lib/mutt`

### 2

$Size = 13 + 256 + 256^2 + 256^3 = 16843021 KB$.

### 3

As 15000 rpm = 250 rps, 262144 bytes = 256KB, one rotation = 1 / 250rps = 4ms, average rotation time = 2ms, transfer rate = 256KB / 4ms = 64000KB/s

(i)

$Data Rate = \frac{1}{10+\frac{1KB}{64000KB/s}} \approx 99 KB/s$

(ii)

$Data Rate = \frac{2}{10+\frac{2KB}{64000KB/s}} \approx 199 KB/s$

(iii)

$Data Rate = \frac{4}{10+\frac{4KB}{64000KB/s}} \approx 399 KB/s$

## Ex. 4

### 1

Number of choices = $26^8 \approx 2.088 \times 10^{11}$. The more patterns containing, the higher the complexity, and lower the possiblility to brute-force the password.

### 2

Because `malloc()` does not initialize memory. If memory comes from a fresh OS page, then it is set to zero. If memory comes from the program's free list, the memory will be random.

### 3

The security of the system depends on the weakest component.

### 4

Theoretically no, absolute security is impossible. Software always has undiscovered bugs. Hardware can have design flaws. Humans are the weakest link. Attackers can constantly invent new methods.

## Ex. 5

Linux OS, we must know that it is a system based on Unix-like. Systems that are called Unix-like or Unix-based are designed to behave and function similarly to the Unix OS. Linux OS comes under the open-source family (code designed to be publicly accessible) and is based on the Linux kernel. It was initially released on September 17, 1991, by Linus Torvalds.

It has a modular design, which helps us with system customization according to our needs. Linux is open-source software that has helped in the growth and development of our developer community which is contributing to the open-source community. It also offers a command line interface for interacting with OS and also has a graphical interface for users who are not used to working with the command line interface.

What is Linux Firewall (firewalld)
A virtual wall in the security system world is designed to protect our system from unwanted traffic and unauthorized access to our system. The security system in Linux OS is known as Linux Firewall, which monitors and governs the network traffic (outbound/inbound connections). It can be used to block access to different IP addresses, Specific subnets, ports (virtual points where network connections begin and end), and services. We have a daemon's name called Firewalld which is used to maintain the firewall policies. A dynamically managed firewall tool in a Linux system is known as Firewalld, it can be updated in real-time if there are any changes in the network environment.

This Firewalld works in concepts of zones (segments). We can check whether our firewall services are running or not by using the commands sudo (user access) and systemctl (use to control and manage the status of services).

```shell
sudo systemctl status firewalld
```

Types of Linux Firewalls

There is more than one Linux firewall option available. When we come to drop down and research, we have a few popular names IPCop, iptables, Shorewall, and UFW But one of the most popular is the "iptables" firewall.

Iptables Working:
Linux-based software that performs manipulation functions, packet filtering, and NAT (network address translation) is known as Iptables. With the help of Iptables which allows system administrators to control incoming and outgoing traffic by setting up the rules. 

When a packet is received in a Linux base system, it has to go through the chains and tables in the iptables firewall. The most commonly used tables are filter and nat but we have five predefined tables in iptables (raw, nat, filter, security, and mangle).

Types of Tables

We will discuss five predefined tables:

Security Table: It is often used in conjunction with other security tools like SELinux, it is also used for MAC (Mandatory Access Control) rules, which can further be used to set rules related to security labels and access controls. It has four built-in chains: OUTPUT, FORWARD, INPUT, and SECMARK.
Mangle Table: It is used to modify packets by setting the packet's ToS/DSCP field, altering packet header fields, and changing packet marks. It has Five built-in chains: POSTROUTING, FORWARD, OUTPUT, PREROUTING, and INPUT.
Nat Table: It stands for network address translation, which helps in sharing a single public IP address between multiple devices. It has two built-in chains: PREROUTING and POSTROUTING.
Raw Table: It is used for the configuration of low-level packet processing. It has limited built-in chains, but the user can create additional chains if required.
Filter Table: It is used for packet filtering. It has three built-in chains. INPUT, OUTPUT, and FORWARD.
Here filters are responsible for filtering the packets on the defined rules based on the source and destination of the IP address, port number, and protocol type. And Chains there are three different types of built-in chains.

(Source: [Linux Firewall](https://www.geeksforgeeks.org/linux-unix/linux-firewall/))
