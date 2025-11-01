#!/bin/bash

FILE="counter.txt"
LOCKFILE="counter.lock"

if [ ! -s "$FILE" ]; then
  echo 0 > "$FILE"
fi

(
  flock -x 200
  last=$(tail -n 1 "$FILE")
  next=$((last + 1))
  echo "$next" >> "$FILE"
) 200>"$LOCKFILE"