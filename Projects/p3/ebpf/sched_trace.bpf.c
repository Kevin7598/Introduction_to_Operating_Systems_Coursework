// ebpf/sched_trace.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

char LICENSE[] SEC("license") = "GPL";

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 4096);
    __type(key, u32);
    __type(value, u64);
} last_in_ns SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 4096);
    __type(key, u32);
    __type(value, u64);
} acc_runtime_ns SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 4096);
    __type(key, u32);
    __type(value, u8);
} runnable SEC(".maps");

SEC("tracepoint/sched/sched_switch")
int trace_sched_switch(struct trace_event_raw_sched_switch *ctx)
{
    u32 prev_pid = ctx->prev_pid;
    u32 next_pid = ctx->next_pid;
    u64 ts = bpf_ktime_get_ns();

    // For prev_pid: compute runtime delta and add to acc_runtime_ns
    u64 *p_last = bpf_map_lookup_elem(&last_in_ns, &prev_pid);
    if (p_last) {
        u64 delta = ts - *p_last;
        u64 *p_acc = bpf_map_lookup_elem(&acc_runtime_ns, &prev_pid);
        if (p_acc) {
            __sync_fetch_and_add(p_acc, delta);
        } else {
            bpf_map_update_elem(&acc_runtime_ns, &prev_pid, &delta, BPF_ANY);
        }
    }

    // update last_in_ns for next_pid (switch-in)
    bpf_map_update_elem(&last_in_ns, &next_pid, &ts, BPF_ANY);

    // mark prev and next as "seen"
    u8 one = 1;
    bpf_map_update_elem(&runnable, &prev_pid, &one, BPF_ANY);
    bpf_map_update_elem(&runnable, &next_pid, &one, BPF_ANY);

    return 0;
}