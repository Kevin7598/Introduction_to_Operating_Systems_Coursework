# LemonDB
[![Release](https://img.shields.io/badge/release-3.0.0-blue.svg)](./CHANGELOG.md)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
![CMake](https://img.shields.io/badge/CMake-%3E%3D3.16-informational)
![Compiler](https://img.shields.io/badge/Compiler-clang--18-orange)
![Build](https://img.shields.io/badge/build-local-green)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
## Installation

### Requirements
- clang-18 (clang and clang++)
- CMake >= 3.16
- GNU make / build tools

### Quick build
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```



### Run
```bash
./build/bin/lemondb
```

## Usage

LemonDB provides an interactive shell. Commands are case-sensitive and generally follow this form:

`OPER ( args ... ) FROM table [WHERE ( KEY = "str" ) ( field OP int ) ...]`

Implemented commands:

- Management
	- LOAD <file>            Load a table file (table name inferred from filename)
	- DUMP <table> <file>    Dump a table to a file (and remember the mapping)
	- LIST                   List all loaded tables
	- SHOWTABLE <table>      Print a table
	- DROP <table>           Drop a table from memory
	- COPYTABLE <table> <newtable>  Create a copy of an existing table
	- TRUNCATE <table>       Clear all records from a table
	- QUIT                   Exit LemonDB

- Data queries
	- `INSERT ( v1 v2 ... ) FROM <table>`
	- `UPDATE ( field value [field value] ... ) FROM <table> WHERE (...)`
	- `SELECT ( field1 field2 ... ) FROM <table> [WHERE (...)]`
	- `DELETE FROM <table> [WHERE (...)]`
	- `DUPLICATE FROM <table> [WHERE (...)]`
	- `SWAP ( field1 field2 ) FROM <table> [WHERE (...)]`
	- `SUM ( field1 field2 ... ) FROM <table> [WHERE (...)]`
	- `COUNT FROM <table> [WHERE (...)]`
	- `MIN ( field1 field2 ... ) FROM <table> [WHERE (...)]`
	- `MAX ( field1 field2 ... ) FROM <table> [WHERE (...)]`
	- `ADD ( fields ... destField ) FROM <table> [WHERE (...)]`
	- `SUB ( fieldSrc fields ... destField ) FROM <table> [WHERE (...)]`

Notes:
- WHERE supports conditions of the form: ( KEY = "string" ) and ( field OP int ), where $\mathrm{OP} \in \{=, \ne, <, \le, >, \ge\}$.
- DUPLICATE creates a copy of each matched row with key suffix "_copy". If a copy already exists, originals are not re-copied; copies of copies are allowed.

### Test Runner
See [README.md](./test/README.md) for instructions on using the automated test runner.

## Features

### Core Query Set
* Table management: `LOAD`, `DUMP`, `LIST`, `SHOWTABLE`, `DROP`, `COPYTABLE`, `TRUNCATE`, `QUIT`
* Data operations: `INSERT`, `UPDATE`, `SELECT`, `DELETE`, `DUPLICATE`, `SWAP`
* Aggregation queries: `SUM`, `COUNT`, `MIN`, `MAX`
* Arithmetic operations: `ADD`, `SUB`

### Multi-Threaded Scheduler
Parallelism at the granularity of WHOLE QUERIES:
* Worker pool executes independent read-only queries concurrently.
* Read batches separated by write (or DB-level) barriers preserve semantic ordering (Read -> Write -> Read epochs).
* DB-level operations (`LOAD`, `DROP`, `COPYTABLE`) act as global barriers across all tables.
* Dependency tracking via sequence numbers and per-table + global metadata (last write, pending reads).
* Streaming ordered output: a dedicated printer thread flushes results in input order using a watermark advanced at each write barrier (reduces memory vs. buffering all results to the end).
* Graceful `QUIT`: no hard `std::exit`; all outputs flushed before termination.

Performance notes (design level):
* Read-only queries on disjoint tables overlap fully.
* Writes never interleave with preceding reads that could observe inconsistent state.
* Output determinism ensured (stable by input sequence) regardless of execution interleaving.


## Roadmap

- [x] Milestone 1: Single-threaded core query engine
- [x] Milestone 2: Multi-threaded query execution (inter-query parallelism: read batches, write & DB-level barriers, ordered streaming output)
- [x] Milestone 3: Performance optimizations & code quality improvements

