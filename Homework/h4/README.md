# ECE4820J Homework 4

### Kaiqi Zhu 522370910091

## Ex. 1

### 1

Problem: The signal handler may try to perform a thread switch or preemption inside the runtime system, which causes inconsistent internal state.

Solution: Block or disable the timer interrupt signal when entering the runtime system and enable it when returning to user threads.

### 2

Yes, it is possible to implement a threads package using alarms to interrupt blocking system calls through using timer signals to forcibly break out of blocking calls in user space.

## Ex. 2

The initial code can be found in [counter.sh](/src/counter.sh).

### 1

2 seconds.

### 2

The modified version can be found [counter_modified.sh](/src/counter_modified.sh).

## Ex. 3

*semaphore.h* can be found in `/usr/include/semaphore.h`. The modified version is in [ex3.c](/src/ex3.c)