# ECE4820J Homework 5

### Kaiqi Zhu 522370910091

## Ex. 1

### 1

No. If both processes are blocked and each still needs at least one more resource, then every resource must be allocated. With 3 identical resources the total allocated would be 3. That allocation can only be split as 2 + 1 (one process holding 2, and the other holding 1). But the process holding 2 has already reached its maximum and thereby cannot be waiting for more and it could run to completion and free resources. Hence it's impossible for both to be blocked waiting simultaneously. Therefore deadlock cannot occur.

### 2

n can be lower than or equal to 5, namely 1, 2, 3, 4, or 5, because for n $\leq$ 6 a subset of 6 processes can create the deadlock, so the system is not guaranteed deadlock-free.

### 3

As 35/50 = 0.70, 20/100 = 0.20, 10/200 = 0.05,

$Sum \leq 1 \rightarrow 0.70 + 0.20 + 0.05 + x/250 \leq 1 \rightarrow 0.95 + x/250 \leq 1 \rightarrow x/250 \leq 0.05 \rightarrow x \leq 12.5 ms.$

So the largest x that still allows schedulability under EDF is 12.5 ms.

### 4

It will receive multiple time-slices per full rotation, which can effectively give it a larger share of CPU time than processes listed once.

Reason: It becomes a simple way to implement weighted round-robin (more entries $\rightarrow$ higher share) without changing quantum length.

### 5

Yes, we can look for tight loops, recursive functions, or nested iterations that perform arithmetic, logical operations, or data transformations, and count the frequency of CPU-heavy instructions. To determine it at runtime, we can use commands like `top` or `htop`, and profiling tools including `perf` and `strace`.

## Ex. 2

### 1

Request Matrix

| Process |  |  |  |
|----------|---|---|---|
| P1 | 7 | 4 | 3 |
| P2 | 1 | 2 | 2 |
| P3 | 6 | 0 | 0 |
| P4 | 0 | 1 | 1 |
| P5 | 4 | 3 | 1 |

### 2

Yes, a possible sequence is $P_2 \rightarrow P_4 \rightarrow P_5 \rightarrow P_1 \rightarrow P_3$, so that the system can be in a safe state.

### 3

Yes, same as the sequence in the previous section.

- P2: Request (1,2,2) $\leq$ (3,3,2). Finish and release (2,0,0).
New Work = (3,3,2) + (2,0,0) = (5,3,2).
- P4: Request (0,1,1) $\leq$ (5,3,2). Finish and release (2,1,1).
New Work = (5,3,2) + (2,1,1) = (7,4,3).
- P5: Request (4,3,1) $\leq$ (7,4,3). Finish and release (0,0,2).
New Work = (7,4,3) + (0,0,2) = (7,4,5).
- P1: Request (7,4,3) $\leq$ (7,4,5). Finish and release (0,1,0).
New Work = (7,4,5) + (0,1,0) = (7,5,5).
- P3: Request (6,0,0) $\leq$ (7,5,5). Finish and release (3,0,2).
Final Work = (7,5,5) + (3,0,2) = (10,5,7).

## Ex. 3

### 1

We can use count to track how many readers are currently active, with the first reader locking the database and the last reader releasing it.

Pseudocode:

```cpp
int count = 0;
semaphore count_lock = 1;
semaphore db_lock = 1;

void read_lock() {
    down(count_lock);
    count++;
    if (count == 1) down(db_lock);
    up(count_lock);
}
```

### 2

All readers can read concurrently and the database remains locked (with writers waiting) until the last reader finishes and count becomes 0. If new readers keep arriving continuously, the database never becomes free for writers. Writers will wait forever.

### 3

When a writer wants to write, stop new readers from entering. Current readers finish normally, then the writer can proceed. Use a new semaphore `read_lock` to block reader entry.

Pseudocode:

```cpp
int count = 0;
semaphore count_lock = 1;
semaphore db_lock = 1;
semaphore read_lock = 1;

void read_lock() {
    down(read_lock);
    down(count_lock);
    count++;
    if (count == 1) down(db_lock);
    up(count_lock);
    up(read_lock);
}

void read_unlock() {
    down(read_lock);
    down(count_lock); 
    count--;
    if (count == 0) up(db_lock);
    up(count_lock);
    up(read_lock);
}

void write_lock() {
    down(read_lock);
    down(db_lock);
}

void write_unlock() {
    up(db_lock);
    up(read_lock);
}
```

### 4

When a writer arrives, it does down(read_lock), preventing new readers from starting. However, if many writers arrive one after another, there will be problems for readers because each writer grabs read_lock again before readers get another chance. The writer has the priority

Accordingly, this solution is writer-priority, preventing writer problems, but potentially causing reader problems. Therefore, the problem is still not solved.
