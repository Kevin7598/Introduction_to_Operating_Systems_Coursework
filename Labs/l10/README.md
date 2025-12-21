# ECE4820J Lab 10

### Kaiqi Zhu 522370910091

## A clean setup

### Where to copy the dice module for it to be officially known to the kernel?

`/lib/modules/$(shell uname -r)/build`

### What command to run in order to generate the modules.dep and map files?

```shell
sudo depmod -a
```

### How to ensure the dice module is loaded at boot time, and how to pass it options?

- To load at boot, add the module name to `/etc/modules-load.d/dice.conf`.
- To pass options, create a file in `/etc/modprobe.d/dice.conf`.

### How to create a new `friends` group and add grandpa and his friends to it?

```bash
sudo groupadd friends
sudo usermod -aG friends grandpa
sudo usermod -aG friends friend1
sudo usermod -aG friends friend2
```

### What is `udev` and how to define rules such that the group and permissions are automatically setup at device creation?

`udev` is the Linux device manager, which creates device files under `/dev`, assigns permissions, ownership, and triggers scripts based on rules.

## Hacking mum's computer

### How to adjust the PATH, ensure its new version is loaded but then forgotten?

```shell
export PATH="/my/new/dir:$PATH"
```

### What is the exact behaviour of su when wrong password is input?

It prompts once, prints `su: Authentication failure`, and exits with non-zero status (1).

### When using the `read` command how to hide the user input?

```shell
read -s PASSWORD
```

### How to send an email from the command line?

Using `sendmail`,

```bash
sendmail recipient@example.com <<EOF
Subject: Title
Body text
EOF
```
