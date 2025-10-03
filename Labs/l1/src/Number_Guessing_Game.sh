#!/bin/bash

target=$((RANDOM % 100))

echo "Guess Game Begins!"

while true; do
    read -p "Enter your guess: " guess

    if ! [[ "$guess" =~ ^[0-9]+$ ]]; then
        echo "Please enter a valid number."
        continue
    fi

    if (( guess < target )); then
        echo "Larger"
    elif (( guess > target )); then
        echo "Smaller"
    else
        echo "Congratulations! You have guessed the right number $target!"
        break
    fi
done