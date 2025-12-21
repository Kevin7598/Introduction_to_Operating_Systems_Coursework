# userspace/clients.py
import os
import subprocess
import time

def get_nice(pid):
    """
    Retrieves the nice value for a given PID.
    Attempts to read from /proc first, falling back to `ps`.
    """
    try:
        with open(f"/proc/{pid}/status", "r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                if line.startswith("Nice:"):
                    parts = line.split()
                    if len(parts) >= 2:
                        try:
                            return int(parts[1])
                        except ValueError:
                            break
    except FileNotFoundError:
        return None
    except Exception:
        pass

    # Fallback to ps command if /proc read fails.
    for _ in range(2):
        try:
            s = subprocess.check_output(["ps", "-o", "ni=", "-p", str(pid)], text=True, timeout=0.1)
            return int(s.strip())
        except subprocess.TimeoutExpired:
            return None
        except subprocess.CalledProcessError:
            time.sleep(0.01)
        except ValueError:
            return None
        except KeyboardInterrupt:
            raise
    return None

def set_affinity(pid, cpus):
    """
    Sets the CPU affinity for the specified PID using taskset.
    """
    mask = 0
    for c in cpus:
        mask |= (1 << c)
    cmd = ["taskset", "-pc", ",".join(str(c) for c in cpus), str(pid)]
    devnull = open(os.devnull, 'w', encoding='utf-8', errors='ignore')
    try:
        subprocess.check_call(cmd, stdout=devnull, stderr=devnull)
    finally:
        devnull.close()

def clear_affinity(pid):
    n = os.cpu_count()
    cpus = list(range(n))
    set_affinity(pid, cpus)