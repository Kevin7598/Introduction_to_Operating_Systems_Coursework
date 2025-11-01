#!/bin/bash

FILE="counter.txt"

if [ ! -s "$FILE" ]; then
  echo 0 > "$FILE"
fi

last=$(tail -n 1 "$FILE")
next=$((last + 1))
echo "$next" >> "$FILE"