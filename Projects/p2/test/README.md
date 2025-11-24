## Test Runner Usage

This directory contains an automated comparison runner (`run.sh`) that executes a set of query scripts against different LemonDB binaries and diffs their stdout against the expected outputs under `stdout/`.

### Directory Layout After Script Starts
The script creates (or reuses) a `data/` working area with symlinks:

```
test/
	run.sh            # the runner script (you invoke this)
	data/
		db -> /opt/lemondb-bck/test/db          # symlink to test database tables
		queries -> /opt/lemondb-bck/test/queries-4820   # symlink to query scripts
		stdout -> /opt/lemondb-bck/test/stdout-4820     # symlink to expected outputs
		tmp/           # per-run temporary output folder
```

Inside `data/tmp/` each test produces an output file named:

```
test-<test_name>.out
```

and it is diffed with:

```
../stdout/<test_name>.out
```


To add or remove tests, edit this array. Each element corresponds to a query file `<name>.query` under `data/queries/` and an expected output `<name>.out` under `data/stdout/`.

### Binaries / Modes
The runner supports three execution modes:

| Mode | Invocation | Binary Path (relative to data/tmp/) | Description |
|------|------------|--------------------------------------|-------------|
| Multithread (default) | `bash run.sh` | `../../../build/bin/lemondb` | Your compiled multi-thread version (uses scheduler & streaming printer). |
| Memory Sanitizer | `bash run.sh -m` | `../../../build/bin/lemondb_memory_check` | The memory sanitizer build for detecting memory errors. |
| Address Sanitizer | `bash run.sh -a` | `../../../build/bin/lemondb-asan` | AddressSanitizer build for detecting memory errors. |

The script resolves the binary *after* changing into `data/tmp/`, so the relative paths above start from that directory.

Ensure the optional binaries exist and are executable if you use `-s` or `-b`. Otherwise the script exits with an error.

### Running Examples
```bash
# Default: multi-thread version
bash test/run.sh

# Memory Sanitizer version
bash test/run.sh -m

# Address Sanitizer version
bash test/run.sh -a
```

### Output Format
For each test the script prints:

1. Test name
2. Timing information from `time`
3. Diff result (PASS / FAIL)

Example snippet:
```
test_listen_2
real 0m0.42s
Test test_listen_2 passed
```


---
Last updated: 2025-11-07