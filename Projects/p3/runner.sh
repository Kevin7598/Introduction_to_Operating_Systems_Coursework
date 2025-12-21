#!/bin/bash
make load-ebpf
python3 userspace/spawn_tasks.py --count 5 --min -5 --max 5

# Trap INT (Ctrl+C) so it stops the python script but continues execution here
trap "echo 'Lottery stopped, cleaning up...'" INT
python3 userspace/lottery.py 10
trap - INT

pkill -f cpu_task
rm -rf /sys/fs/bpf/sched_trace/*
make unload-ebpf

echo "Cleanup complete."