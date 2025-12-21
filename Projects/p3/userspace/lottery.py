#!/usr/bin/env python3
# userspace/lottery.py
import subprocess, random, time, re, sys, os, shutil, json, pwd
from clients import get_nice, set_affinity, clear_affinity

PINNED_MAP_PATH = "/sys/fs/bpf/sched_trace/runnable"

# Locate bpftool executable
BPFTOOL = shutil.which("bpftool")
if not BPFTOOL:
    # Check common locations if not in PATH.
    for p in ("/usr/bin/bpftool", "/usr/sbin/bpftool", "/bin/bpftool"):
        if os.path.exists(p):
            BPFTOOL = p
            break

def dump_map_keys(pinned_path, debug=False):
    """
    Retrieves all keys (PIDs) from the pinned BPF map.
    """
    if not os.path.exists(pinned_path):
        return []

    if not BPFTOOL:
        print("bpftool not found in PATH; please install bpftool or ensure it's reachable")
        return []
    try:
        # Execute bpftool to dump map in JSON format.
        if os.geteuid() == 0:
            cmd = [BPFTOOL, "-j", "map", "dump", "pinned", pinned_path]
        else:
            cmd = ["sudo", BPFTOOL, "-j", "map", "dump", "pinned", pinned_path]
        out = subprocess.check_output(cmd, text=True, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        print("bpftool failed:", e.output.strip() if hasattr(e, 'output') else e)
        return []
    except FileNotFoundError:
        print("bpftool not found; please install bpftool")
        return []
    except Exception as e:
        print("bpftool error:", e)
        return []

    pids = []
    try:# Parse JSON output to extract keys.
        
        obj = json.loads(out)
        if isinstance(obj, list):
            for entry in obj:
                if isinstance(entry, dict):
                    pid = None
                    if 'formatted' in entry and isinstance(entry['formatted'], dict):
                        pid = entry['formatted'].get('key')
                    elif 'key' in entry:
                        k = entry['key']
                        try:
                            pid = int(k)
                        except Exception:
                            pass
                    
                    if pid is not None:
                        try:
                            pids.append(int(pid))
                        except Exception:
                            pass
    except Exception as e:
        if debug:
            print(f"DEBUG: JSON parse failed: {e}")
        pass

    if not pids:
        for line in out.splitlines():
            m = re.search(r"key[\"']?\s*[:=]\s*0x([0-9a-fA-F]+)", line)
            if m:
                try:
                    pid = int(m.group(1), 16)
                    pids.append(pid)
                    continue
                except Exception:
                    continue
            m2 = re.search(r"key[\"']?\s*[:=]\s*([0-9]+)", line)
            if m2:
                try:
                    pid = int(m2.group(1))
                    pids.append(pid)
                except Exception:
                    pass

    return pids

def nice_to_tickets(nice):
    if nice is None: return 10
    scale = 200
    idx = nice + 20
    tickets = max(1, scale - int((scale-1) * idx / 39))
    return tickets

def pick_lottery(candidates):
    tickets = []
    
    # Weighted random selection.
    total = 0
    for pid in candidates:
        n = get_nice(pid)
        if n is None:
            continue
        t = nice_to_tickets(n)
        tickets.append((pid, t))
        total += t
    if total == 0: return None
    r = random.randint(1, total)
    s = 0
    for pid, t in tickets:
        s += t
        if r <= s: return pid
    return None

def get_proc_info(pid):
    info = {
        'pid': pid,
        'uid': None,
        'nice': None,
        'comm': None,
        'cmdline': None,
        'etime': None,
    }
    try:
        st = os.stat(f"/proc/{pid}")
        info['uid'] = st.st_uid
    except Exception:
        return info

    try:
        info['nice'] = get_nice(pid)
    except Exception:
        info['nice'] = None

    try:
        with open(f"/proc/{pid}/comm","r",encoding='utf-8',errors='ignore') as f:
            info['comm'] = f.read().strip()
    except Exception:
        info['comm'] = None
    try:
        with open(f"/proc/{pid}/cmdline","rb") as f:
            raw = f.read()
            if raw:
                info['cmdline'] = raw.replace(b'\x00', b' ').decode('utf-8', errors='ignore').strip()
            else:
                info['cmdline'] = ''
    except Exception:
        info['cmdline'] = None

    try:
        out = subprocess.check_output(["ps","-p",str(pid),"-o","etime="], text=True, stderr=subprocess.DEVNULL)
        info['etime'] = out.strip()
    except Exception:
        info['etime'] = None

    return info

def process_exists(pid):
    try:
        os.kill(pid, 0)
        return True
    except OSError:
        return False

def clean_dead_pids_from_map(pinned_path):
    if not BPFTOOL or not os.path.exists(pinned_path):
        return
    try:
        pids = dump_map_keys(pinned_path)
        count = 0
        for pid in pids[:50]:
            if not process_exists(pid):
                try:
                    if os.geteuid() == 0:
                        cmd = [BPFTOOL, "map", "delete", "pinned", pinned_path, "key", str(pid).encode().hex()]
                    else:
                        cmd = ["sudo", BPFTOOL, "map", "delete", "pinned", pinned_path, "key", str(pid).encode().hex()]
                    subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                    count += 1
                except: pass
                if count >= 10:
                    break
    except: pass

def set_forced_run(winner, candidates):
    ncpu = os.cpu_count() or 1
    if ncpu == 1:
        return
    try:
        if process_exists(winner):
            try:
                set_affinity(winner, [0])
            except: pass
        for pid in candidates:
            if pid == winner: continue
            if not process_exists(pid):
                continue
            try:
                set_affinity(pid, [1 % ncpu])
            except: pass
    except: pass

def main(loop_ms=50, debug=False):
    print("Lottery scheduler started (loop %d ms)." % loop_ms)
    print(f"Using pinned map: {PINNED_MAP_PATH}")

    if not os.path.exists(PINNED_MAP_PATH):
        print(f"ERROR: Map not found at {PINNED_MAP_PATH}")
        print("Please run: make load-ebpf")
        return

    winner_stats = {}
    total_rounds = 0
    candidate_counts = []

    try:
        clean_counter = 0
        first_iter = True
        while True:
            cand = dump_map_keys(PINNED_MAP_PATH, debug=first_iter)
            first_iter = False
            
            if debug:
                print(f"debug: read {len(cand)} keys, sample={cand[:10]}")

            if not cand:
                time.sleep(loop_ms/1000.0)
                clean_counter += 1
                continue
            
            cand = [p for p in cand if p != 0]

            try:
                real_user = os.environ.get('SUDO_USER') or os.environ.get('USER')
                if real_user:
                    real_uid = pwd.getpwnam(real_user).pw_uid
                else:
                    real_uid = os.geteuid()
            except Exception:
                real_uid = os.geteuid()

            visible = []
            for p in cand:
                if not process_exists(p):
                    continue
                proc_path = f"/proc/{p}"
                if not os.path.exists(proc_path):
                    continue
                try:
                    if os.stat(proc_path).st_uid != real_uid:
                        continue
                except Exception:
                    continue
                visible.append(p)

            if visible:
                cand = visible

            candidate_counts.append(len(cand))
            total_rounds += 1

            winner = pick_lottery(cand)
            
            if winner:
                info = get_proc_info(winner)
                nice = info.get('nice')
                tickets = nice_to_tickets(nice)
                comm = info.get('comm') or ''
                cmdline = info.get('cmdline') or ''
                uid = info.get('uid')
                etime = info.get('etime') or ''
                source = 'cpu_task' if ('cpu_task' in comm) or ('cpu_task' in cmdline) else 'other'

                print(f"[{time.strftime('%H:%M:%S')}] Winner: PID {winner}  ({len(cand)} candidates)  tickets={tickets}")
                print(f"  UID={uid} nice={nice} comm={comm} elapsed={etime} source={source}")
                if cmdline:
                    print(f"  cmd: {cmdline}")

                if winner not in winner_stats:
                    winner_stats[winner] = {
                        'count': 0,
                        'nice': nice,
                        'comm': comm,
                        'tickets': tickets,
                        'source': source,
                    }
                winner_stats[winner]['count'] += 1

                set_forced_run(winner, cand)
            
            clean_counter += 1
            if clean_counter >= 20:
                clean_counter = 0
                clean_dead_pids_from_map(PINNED_MAP_PATH)
            
            time.sleep(loop_ms/1000.0)
    except KeyboardInterrupt:
        print("\nExiting, clearing affinities")
        for pid in dump_map_keys(PINNED_MAP_PATH):
            try:
                clear_affinity(pid)
            except: pass

        print("\n" + "="*80)
        print("STATISTICS")
        print("="*80)
        print(f"Total rounds: {total_rounds}")
        if candidate_counts:
            avg_cand = sum(candidate_counts) / len(candidate_counts)
            print(f"Average candidate set size: {avg_cand:.1f}")
        
        cpu_task_count = 0
        other_count = 0
        cpu_task_stats = []
        other_stats = []
        
        for pid, stats in sorted(winner_stats.items(), key=lambda x: x[1]['count'], reverse=True):
            source = stats['source']
            count = stats['count']
            nice = stats['nice']
            tickets = stats['tickets']
            comm = stats['comm']
            
            if source == 'cpu_task':
                cpu_task_count += count
                cpu_task_stats.append((pid, count, nice, tickets, comm))
            else:
                other_count += count
                other_stats.append((pid, count, nice, tickets, comm))
        
        total_wins = cpu_task_count + other_count

        if total_wins > 0:
            print(f"\ncpu_task processes: {cpu_task_count}/{total_wins} wins ({100*cpu_task_count/total_wins:.1f}%)")
            print(f"other processes:    {other_count}/{total_wins} wins ({100*other_count/total_wins:.1f}%)")
        else:
            print("\nNo winners recorded.")
        
        print("\nTop cpu_task winners:")
        for pid, count, nice, tickets, comm in sorted(cpu_task_stats, key=lambda x: x[1], reverse=True)[:10]:
            pct = 100*count/total_wins if total_wins > 0 else 0
            print(f"  PID {pid:5d}: {count:3d} wins ({pct:5.1f}%)  nice={str(nice):>4}  tickets={tickets:3d}  comm={comm}")
        
        print("\nTop other winners:")
        for pid, count, nice, tickets, comm in sorted(other_stats, key=lambda x: x[1], reverse=True)[:10]:
            pct = 100*count/total_wins if total_wins > 0 else 0
            print(f"  PID {pid:5d}: {count:3d} wins ({pct:5.1f}%)  nice={str(nice):>4}  tickets={tickets:3d}  comm={comm}")
        
        print("\n" + "="*80)

if __name__ == "__main__":
    if os.geteuid() != 0:
        print("Warning: script calls sudo bpftool internally, but root is recommended.")
    ms = 50
    debug = False
    args = [a for a in sys.argv[1:]]
    if "--debug" in args:
        debug = True
        args.remove("--debug")

    if len(args) > 0:
        try:
            ms = int(args[0])
        except ValueError:
            pass

    main(ms, debug=debug)