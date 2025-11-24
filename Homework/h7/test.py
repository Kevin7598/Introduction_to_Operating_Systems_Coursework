from bcc import BPF

# BPF program
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

# attach kprobe to do_fork()
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