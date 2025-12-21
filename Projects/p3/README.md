# Lottery Scheduler

![Language: C](https://img.shields.io/badge/Language-C-blue)
![Language: Python](https://img.shields.io/badge/Language-Python-blue)
![Platform: Linux](https://img.shields.io/badge/Platform-Linux-lightgrey)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

## Overview

This project demonstrates a lottery scheduling algorithm using eBPF tracepoints and userspace interaction. An eBPF program tracks runnable processes in a pinned map, while a userspace Python agent samples this map to select a "winner" process based on ticket weights derived from process niceness.

## Prerequisites

*   **OS**: Linux kernel 5.x+ (requires eBPF tracepoint support).
*   **Tools**: `clang` (target bpf), `llvm`, `make`, `gcc`.
*   **Utilities**: `bpftool` (essential), `libbpf-tools`.
*   **Runtime**: Python 3.8+.
*   **Privileges**: Root access is required for loading eBPF programs and map management.

**Debian/Ubuntu Install:**
```bash
sudo apt update && sudo apt install -y clang gcc make libbpf-tools bpftool python3
```

## Build and Installation

1.  **Build Binaries**: Compiles the eBPF object and the test workload.
    ```bash
    make build-ebpf build-tests
    ```

2.  **Load eBPF Program**: Mounts the BPF filesystem, loads the program, and pins maps to `/sys/fs/bpf/sched_trace`.
    ```bash
    sudo make load-ebpf
    ```

## Easy Test
With `runner.sh`, you can quickly spawn test workloads and run the lottery scheduler.

```bash
sudo bash runner.sh
```

## Detailed Usage

### 1. Spawn Workload (Optional)
Generate background CPU-intensive tasks to serve as lottery candidates.
```bash
# Spawn 5 tasks with niceness ranging from -5 to 5
sudo python3 userspace/spawn_tasks.py --count 5 --min -5 --max 5

# Remember to terminate these tasks after testing:
pkill -f cpu_task
```

### 2. Run Scheduler
Execute the userspace lottery agent.
```bash
# Run with default settings (500ms loop)
sudo python3 userspace/lottery.py 500

# Run with debug output
sudo python3 userspace/lottery.py 50 --debug
```

### 3. Reset State
To ensure a clean experimental environment, clear the pinned BPF maps and reload.
```bash
sudo rm -rf /sys/fs/bpf/sched_trace/*
make unload-ebpf
sudo make load-ebpf
```

## Repository Structure

*   `ebpf/`: eBPF C source code (`sched_trace.bpf.c`).
*   `userspace/`: Python control scripts (`lottery.py`, `spawn_tasks.py`).
*   `tests/`: C source for the dummy workload (`cpu_task`).
*   `Makefile`: Build automation targets.

## Troubleshooting

*   **`bpftool` not found**: Verify installation and ensure it is in your `$PATH`.
*   **Permission Denied**: Operations involving eBPF loading, map pinning, and negative niceness require `sudo`.
*   **Empty Candidate Set**: Ensure the spawner and lottery script run under the same user context (or root) to ensure visibility.
