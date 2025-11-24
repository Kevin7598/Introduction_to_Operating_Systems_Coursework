
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.0.0] - 2025-11-22
### Added
- **Milestone 3 Completion**: Performance optimizations & code quality improvements.
  - Applied `clang-tidy`, `clang-format`, `cpplint` and `cppcheck` for consistent style and to fix potential issues.
  - Refactored codebase for better readability and maintainability.
  - Improved documentation in code comments and README.md.
  - Enhanced build system for easier compilation and testing, including `run.sh` script for automated testing.
  - Fix bugs in some functions to improve performance

## [2.0.0] - 2025-11-07
### Added
- **Milestone 2 Completion**: Inter-query multi-threaded execution layer (scheduler).
  - Worker pool executing parsed query tasks concurrently.
  - Read parallelism: multiple read-only queries run together when no intervening write hazard.
  - Per-table + global dependency tracking: last write, pending reads, global barrier sequence.
  - DB-level operation classification (`LOAD`, `DROP`, `COPYTABLE`) introducing global serialization points.
  - Streaming ordered output printer thread with watermark flush at each write/barrier.
  - Graceful `QUIT` (removed immediate `std::exit(0)`) ensuring all buffered outputs are emitted.
  - Query introspection helpers (`isLoad`, `isDbLevel`, `isQuit`, `getTargetTable`).
  - Refactored scheduling logic into `QueryScheduler` (scheduler.cpp / scheduler.h) from monolithic `main.cpp`.
  - Memory reduction via incremental result flushing (rather than end-of-run bulk print).
- **Build Targets**: Distinct release, debug, and sanitizer (memory check) executables.

### Changed
- `Database::exit()` now a no-op to support ordered shutdown & final flush.
- Introduced global barrier semantics to prevent reordering across schema-altering operations.
- README updated with architecture overview and Milestone 3 plans (intra-query parallelism via table sharding & map-reduce style aggregation).

### Fixed
- **Code Quality**: `clang-tidy`, `clang-format`, `cpplint` and `cppcheck` applied for consistent style and to fix potential issues

## [1.0.0] - 2025-10-25

### Added
- **Milestone 1 Completion**: All core query functionality implemented
- **Data Manipulation Instructions**:
  - **DELETE**: Remove records from tables with conditional filtering
  - **SWAP**: Exchange values between two fields with optional conditions
  - **SUB**: Perform arithmetic subtraction on fields and store results
- **Aggregation Queries**:
  - **COUNT**: Count records matching specified conditions
  - **MAX**: Find maximum values in specified fields
- **Table Operations**:
  - **COPYTABLE**: Create complete copies of existing tables
  - **TRUNCATE**: Clear all records while maintaining table structure



## [0.1.0] - 2025-10-24

### Added
- Data Manipulation Instructions
  - **Duplicate**: Creates a copy of rows that match a given condition. A new key is generated for each copied row by appending `_copy`. The logic prevents re-copying an original row if a copy already exists, but allows copies of copies (e.g., `KEY_copy_copy`).
  - **Sum**: Calculates the sum of values in specified integer columns for rows that match a given condition.

<!-- ### Notes -->

