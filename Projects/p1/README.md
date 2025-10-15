![License](https://img.shields.io/badge/finished-SJTU-blue.svg)
[![Build Status](https://focs.ji.sjtu.edu.cn/git/ece482/Zhukaiqi522370910091-p1/actions/workflows/push.yaml/badge.svg?branch=master)](https://focs.ji.sjtu.edu.cn/git/ece482/Zhukaiqi522370910091-p1/actions)

# The Mumsh Shell

A simple Linux-like shell supporting basic command execution, pipelines, input/output redirection, and built-in commands (`cd`, `pwd`, `exit`).

## Build Instructions

To compile the project, run:

```bash
make
```

Or use gcc directly:

```bash
gcc -o main src/*.c
```

## Usage

After building, start the shell with:

```bash
./main
```

You will see the prompt:

```
mumsh $
```

## Features

- Execute external commands (`ls`, `cat`, `echo`, etc.)
- Pipeline support (`ls | grep txt`)
- Input redirection `<`, output redirection `>`, and append `>>`
- Built-in commands:
  - `cd [dir]`: Change current directory
  - `pwd`: Print current working directory
  - `exit`: Exit the shell

## Examples

```bash
mumsh $ ls -l
mumsh $ cd /tmp
mumsh $ pwd
mumsh $ cat file.txt | grep hello > result.txt
mumsh $ exit
```

## Notes

- Press `Ctrl+C` to interrupt the current command (shell will not exit).
- Type `exit` or press `Ctrl+D` to quit the shell.
