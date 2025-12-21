#!/usr/bin/env python3
"""
spawn_tasks.py

Spawns multiple instances of `./tests/cpu_task` with randomized nice values.

Usage:
  # spawn 5 tasks with niceness in [0,10]
  python3 userspace/spawn_tasks.py --count 5 --min 0 --max 10

  # spawn 5 tasks with niceness in [-5,5] (requires sudo)
  sudo python3 userspace/spawn_tasks.py --count 5 --min -5 --max 5
"""
import argparse
import os
import random
import subprocess
import time
import resource


def spawn_one(nice_val, exe="./tests/cpu_task", uid=None, gid=None):
    """
    Spawns a single process with the specified nice value.
    Sets the process group and user ID if provided.
    """
    def _setup_child():
        try:
            # Set the nice value for the new process.
            os.setpriority(os.PRIO_PROCESS, 0, nice_val)
        except (OSError, AttributeError):
            pass
        
        if gid is not None:
            try:
                os.setgid(gid)
            except OSError:
                pass
        if uid is not None:
            try:
                os.setuid(uid)
            except OSError:
                pass

    try:
        # Execute the process.
        p = subprocess.Popen([exe], preexec_fn=_setup_child)
        return p.pid
    except Exception:
        # Fallback if preexec_fn fails.
        p = subprocess.Popen([exe])
        return p.pid


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--count", type=int, default=5, help="number of tasks to spawn")
    parser.add_argument("--min", type=int, default=0, help="min nice value (inclusive)")
    parser.add_argument("--max", type=int, default=10, help="max nice value (inclusive)")
    parser.add_argument("--exe", default="./tests/cpu_task", help="path to test executable")
    args = parser.parse_args()

    if not os.path.exists(args.exe):
        print(f"Error: executable not found: {args.exe}")
        return

    # Determine target uid/gid from SUDO_USER if running as root.
    # This ensures spawned tasks are owned by the user, not root.
    target_uid = None
    target_gid = None
    if os.geteuid() == 0:
        sudo_user = os.environ.get('SUDO_USER')
        if sudo_user:
            import pwd
            pw = pwd.getpwnam(sudo_user)
            target_uid = pw.pw_uid
            target_gid = pw.pw_gid

    spawned = []
    for _ in range(args.count):
        nice_val = random.randint(args.min, args.max)
        pid = spawn_one(nice_val, exe=args.exe, uid=target_uid, gid=target_gid)
        spawned.append((pid, nice_val))
        print(f"spawned pid={pid} nice={nice_val}")
        time.sleep(0.05)

    print("Done. Processes started.")


if __name__ == '__main__':
    main()
