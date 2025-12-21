# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.0.0] - 2025-12-16
### Added
- **Stability & Robustness:**
  - Implemented robust JSON parsing for `bpftool` output in `dump_map_keys()` to handle `formatted.key` and prevent crashes on different system configurations.
  - Added `clean_dead_pids_from_map()` to automatically remove terminated processes from the BPF map, preventing stale candidates.
  - Added candidate filtering to prioritize processes visible to the real user (`SUDO_USER`), improving output relevance.
- **Metrics & Analysis:**
  - Added comprehensive statistics summary on exit (Ctrl-C), including:
    - Total rounds and average candidate set size.
    - Win rates broken down by process source (`cpu_task` vs others).
    - Top winners list with nice values and ticket counts.

### Changed
- Improved error handling when `bpftool` is missing or fails.
- Refactored the main loop to include periodic maintenance tasks (dead PID cleanup).

## [2.0.0] - 2025-12-08
### Added
- Project roadmap outlining 3 key milestones: Stability, Experiment Platform, and Performance.
- Basic userspace driver (`lottery.py`) with ticket calculation based on nice values.
- Helper utilities for CPU affinity management (`clients.py`).

## [1.0.0] - 2025-12-05
### Added
- Initial release of the eBPF lottery scheduler.
- Core BPF program for context switch interception.
- Basic map pinning and userspace loader.