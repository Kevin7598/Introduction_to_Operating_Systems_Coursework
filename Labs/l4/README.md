# ECE4820J Lab 4

### Kaiqi Zhu 522379010091

## Database

### Database system installation

#### Most common database systems

- PostgreSQL
- MySQL
- SQLite

#### Pros and cons

##### PostgreSQL

- Pros: Feature-rich, strong consistency and standards compliance, good performance for complex queries, extensible, and active open-source ecosystem and strong community.

- Cons: Slightly heavier to administer than MySQL for simple use cases, for some extremely high-write/low-latency workloads may require more architecture work, default tuning is conservative, which needs adjustments for large deployments.

##### MySQL

- Pros: Very widely deployed, simple to get started, good read performance, many hosting/managed options, lightweight admin for basic use cases, many tools and ecosystem.

- Cons: Historically weaker SQL standard compliance and fewer advanced features, different storage engines mean behavior can vary, some advanced features are less capable than PostgreSQL.

##### SQLite

- Pros: Serverless, zero-configuration, very small footprint, fast for local single-process reads/writes; ACID with proper journaling modes, extremely easy to embed and distribute.

- Cons: Not designed for high-concurrency writes or large multi-user server workloads, limited built-in scalability, fewer advanced server-side admin features.

### Database queries

#### Top five contributors

``` sql
SELECT author_name, COUNT(*) AS commit_count
FROM commits
GROUP BY author_name
ORDER BY commit_count DESC
LIMIT 5;
```

Results: 
Linus Torvalds|30702
David S. Miller|13180
Takashi Iwai|7726
Mark Brown|7670
Arnd Bergmann|7520

#### Top five contributors in the past five years

The latest data is in 2020.

```sql
SELECT year, author_name, commit_count
FROM (
    SELECT strftime('%Y', commit_date_iso) AS year,
           author_name,
           COUNT(*) AS commit_count,
           ROW_NUMBER() OVER (PARTITION BY strftime('%Y', commit_date_iso) ORDER BY COUNT(*) DESC) AS rn
    FROM commits
    GROUP BY year, author_name
) 
WHERE rn <= 5;
```

Results:
2020|Linus Torvalds|1886
2020|David S. Miller|924
2020|Christoph Hellwig|806
2020|Mauro Carvalho Chehab|770
2020|Chris Wilson|644

#### The most common "commit subject"

Additional steps are needed to preprocess db.csv

```python
with open(input_file, 'r', encoding='utf-8', errors='replace') as f:
    reader = csv.reader(f, delimiter='|')
    for row in reader:
        if len(row) < 3:
            continue
        commit_hash = row[0]
        author_name = row[1]
        commit_subject = '|'.join(row[2:])
        commit_subject = commit_subject.replace('"', '""')
        commit_subject = f'"{commit_subject}"'

        c.execute(f"""
        INSERT OR IGNORE INTO {table_name} (commit_hash, author_name, commit_subject)
        VALUES (?, ?, ?)
        """, (commit_hash, author_name, commit_subject))

conn.commit()
```

```sql
result = conn.execute(query).fetchall()
print("Most common commit subject:")
for row in result:
    print(row[0], "| count:", row[1])
```

Results: "Merge git://git.kernel.org/pub/scm/linux/kernel/git/davem/net" | count: 670

#### The day with the highest number of commits

```sql
sqlite> SELECT date(commit_date_iso) AS commit_day, COUNT(*) AS cnt
FROM commits
GROUP BY commit_day
ORDER BY cnt DESC
LIMIT 1;
```

Results: 2008-01-30|1031

#### The average time between two commits for the five main contributors

```sql
WITH top5 AS (
    SELECT author_name
    FROM commits
    GROUP BY author_name
    ORDER BY COUNT(*) DESC
    LIMIT 5
),
author_commits AS (
    SELECT author_name, commit_date_unix
    FROM commits
    WHERE author_name IN (SELECT author_name FROM top5)
    ORDER BY author_name, commit_date_unix
)
SELECT author_name, AVG(commit_date_unix - prev_commit) AS avg_seconds
FROM (
    SELECT author_name, commit_date_unix,
           LAG(commit_date_unix) OVER (PARTITION BY author_name ORDER BY commit_date_unix) AS prev_commit
    FROM author_commits
)
WHERE prev_commit IS NOT NULL
GROUP BY author_name;
```

Results:
Arnd Bergmann|63807.0030589174
David S. Miller|36956.1432582138
Linus Torvalds|15878.594267101
Mark Brown|59933.2400573738
Takashi Iwai|63301.1109385113

## Debugging

### How to enable built-in debugging in gcc?

Example:

```bash
gcc -g -o myprogram myprogram.c
```

### The meaning of GDB

It is a command-line debugger for programs compiled with GCC (or other GNU toolchain compilers), which can help to step through your program line by line, inspect variable values, set breakpoints and watchpoints, inspect call stacks, and modify variables at runtime.

### The homepage of the GDB project.

[This link](https://www.gnu.org/software/gdb)

### Languages supported by GDB

C, C++, Fortran, Go, Rust, Assembly
