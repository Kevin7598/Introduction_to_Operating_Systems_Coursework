#!/bin/bash
# Build
cd "$(dirname "$0")"
cd ../
mkdir -p build/
cd build/

echo "[BUILD] compiling..."
cmake -DCMAKE_BUILD_TYPE=Release .. > /dev/null 2>&1
make -j$(nproc) > /dev/null 2>&1

####################################
####################################
# Test
cd "$(dirname "$0")"
set -e

# Run all tests by default
tests_arr=()
# select tests to run
# tests_arr=(test_listen_2 test_listen_6 few_insert_delete single_read_dup)

mkdir -p data/
cd data/

ln -sfn /opt/lemondb-bck/test/db db
ln -sfn /opt/lemondb-bck/test/queries-4820 queries
ln -sfn /opt/lemondb-bck/test/stdout-4820 stdout
echo "db -> $(readlink -f db)"

mkdir -p tmp/
cd tmp/

LEMONDB_BIN="../../../build/bin/lemondb"
if [[ "$1" == "-m" ]]; then
    echo -e "\e[32mUsing MSan\e[0m"
    LEMONDB_BIN="../../../build/bin/lemondb_memory_check"
    if [[ ! -x "$LEMONDB_BIN" ]]; then
        echo "lemondb_memory_check binary not found." >&2
        exit 1
    fi
fi

if [[ "$1" == "-a" ]]; then
    echo -e "\e[32mUsing ASan\e[0m"
    LEMONDB_BIN="../../../build/bin/lemondb-asan"
    if [[ ! -x "$LEMONDB_BIN" ]]; then
        echo "lemondb binary not found." >&2
        exit 1
    fi
fi

if [[ ! -x "$LEMONDB_BIN" ]]; then
    echo "lemondb binary not found. Expected ./lemondb or ../../build/bin/lemondb" >&2
    exit 1
fi

QUERIES_DIR="../queries"
if [[ ! -d "$QUERIES_DIR" ]]; then
    if [[ -d "/opt/lemondb-bck/test/queries" ]]; then
        QUERIES_DIR="/opt/lemondb-bck/test/queries"
    else
        echo "Queries directory not found: ./queries or /opt/lemondb-bck/test/queries" >&2
        exit 1
    fi
fi

# Find all .query files and extract base names
if tests_arr=(); then
    for query_file in "$QUERIES_DIR"/*.query; do
        if [[ -f "$query_file" ]]; then
            basename_file=$(basename "$query_file" .query)
            tests_arr+=("$basename_file")
        fi
    done

    if [[ ${#tests_arr[@]} -eq 0 ]]; then
        echo "No .query files found in $QUERIES_DIR" >&2
        exit 1
    fi
fi

echo "Found ${#tests_arr[@]} test(s) to run"

for test in "${tests_arr[@]}";
do
    echo $test
    time "$LEMONDB_BIN" --listen "$QUERIES_DIR/$test.query" > "test-$test.out"
    if ! diff -u "test-$test.out" "../stdout/$test.out"; then
        echo -e "\e[31mTest $test failed\e[0m"
    else
        echo -e "\e[32mTest $test passed\e[0m\n"
    fi
done