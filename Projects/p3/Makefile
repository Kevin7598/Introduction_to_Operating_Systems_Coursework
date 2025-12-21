# Makefile
EBPF_C=ebpf/sched_trace.bpf.c
EBPF_O=sched_trace.bpf.o
BPF_PIN_DIR=/sys/fs/bpf/sched_trace

.PHONY: all build-ebpf load-ebpf unload-ebpf build-tests run-lottery mount-fs

all: build-ebpf build-tests

mount-fs:
	sudo mount -t bpf bpf /sys/fs/bpf || true

build-ebpf:
	clang -O2 -g -target bpf -c $(EBPF_C) -o $(EBPF_O)

load-ebpf: mount-fs build-ebpf
	sudo mkdir -p $(BPF_PIN_DIR)
	-sudo rm -f $(BPF_PIN_DIR)/*
	sudo bpftool prog loadall $(EBPF_O) $(BPF_PIN_DIR) autoattach pinmaps $(BPF_PIN_DIR)

unload-ebpf:
	-sudo rm -rf $(BPF_PIN_DIR)

build-tests:
	gcc -O2 tests/cpu_task.c -o tests/cpu_task

run-lottery:
	sudo python3 userspace/lottery.py 500