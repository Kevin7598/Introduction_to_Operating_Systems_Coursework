# ECE4820J Lab 2 Report

Kaiqi Zhu 522370910091

## Linux Kernel Compilation

1. What is a kernel?

The kernel is the core component of an operating system. It acts as a bridge between hardware and software, managing resources and providing services for applications.

Main roles:

- Process management: scheduling CPU time.

- Memory management: allocating RAM, virtual memory.

- Device management: interfacing with hardware via drivers.

- System calls: providing controlled access to hardware and OS services.

2. What basic tools are necessary to compile the Linux kernel?

- Compiler: gcc (C compiler)

- Make utility: make (or ninja)

- Libraries & headers: libncurses-dev (for menuconfig), bc, flex, bison, libssl-dev

3. List all the commands necessary to compile the kernel.

Step 1. Install the Required Tools for sched_ext

First, install the required packages. The packages in the first section are also required for sched_ext.

```bash
sudo apt update
sudo apt install build-essential libncurses-dev bison flex libssl-dev libelf-dev dwarves
```

Next, install clang-18.

```bash
sudo apt install clang lld
```

Step 2. Download the Kernel

To download the kernel with sched_ext, use:

```bash
wget -v https://git.kernel.org/pub/scm/linux/kernel/git/tj/sched_ext.git/snapshot/sched_ext-for-6.11.tar.gz
tar -zxvf sched_ext-for-6.11.tar.gz
cd sched_ext-for-6.11
```

Step 3. Configure the Kernel

```bash
cp /boot/config-$(uname -r) .config
```

One way to configure the kernel is to use the following command to load the configuration from a file:

```bash
scripts/config --disable SYSTEM_TRUSTED_KEYS
scripts/config --disable SYSTEM_REVOCATION_KEYS
```

Step 4. Compile the Kernel


```bash
make CC=clang -j$(nproc)

sudo make modules_install
sudo make install
```
The `-j$(nproc)` option is to use all the cores of your CPU to compile the kernel. It took me around 1 hour with 16 processors to complete all the three steps.

Step 5. Reboot

```bash
sudo reboot
```

Step 6. Check the Kernel Version

```bash
uname -r
```

4. Why is it recommended to start with the Linux config file from your distribution?

The distro config (```/boot/config-$(uname -r)```) is already tuned for the hardware, which avoids unnecessary features that would slow compilation and reduces risk of breaking drivers or modules.

5. Compile the kernel. How long did it take?

Around 2 hours.

6. Why is code quality so critical at the kernel level?

Kernel runs in privileged mode (ring 0), so bugs can crash the whole system.

Poor code can cause:

- Kernel panics (system crashes).

- Security vulnerabilities (root exploits).

- Data corruption.

There is no safety net like user-space memory protection for kernel code.

## Software Development

1. What is CI/CD?

CI/CD stands for Current Integration and Current Delivery.

Current Integration (CI): Developers frequently merge their code into a shared repository. Each merge triggers an automated build and test pipeline to catch bugs early and ensure new changes integrate smoothly.

Current Delivery (CD): After passing tests, the code is automatically prepared for release but might require manual approval to go live.

2. Why is having a consistent coding style important in industry?

A consistent coding style is crucial because:

- Readability: Code is easier to read and understand, even by someone who did not write it.

- Maintainability: Teams can more easily debug, update, and extend the codebase.

- Collaboration: Multiple developers can work on the same project without confusion.

- Fewer errors: Consistent structure reduces logical and syntax errors.

- Tooling: Automated tools can run smoothly.

3. What is the benefit of sanitizers?

- Catch subtle bugs early (before they reach production).

- Improve security by detecting exploitable memory errors.

- Save debugging time by providing detailed reports of where and how errors occur.

## Remote Work

- The default SSH port is 22.

- File in the ```$HOME/.ssh``` directory: authorized_keys

    - Role: list public keys allowed to log in.

- Key-only authentication workflow in SSH:

    - Client generates a key pair (private + public).

    - The public key is copied to the server (```~/.ssh/authorized_keys```).

    - When logging in, the server challenges the client.

    - The client proves it has the private key.

    - If successful, login is granted, and password no longer required.
    