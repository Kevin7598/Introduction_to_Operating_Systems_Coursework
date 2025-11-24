# ECE4820J Homework 6

### Kaiqi Zhu 52370910091

## Ex. 1

### 1

#### First Fit:

- 12KB -> 20KB
- 10KB -> 10KB
- 9KB -> 18KB

#### Best Fit:

- 12KB -> 12KB
- 10KB -> 10KB
- 9KB -> 9KB

#### Quick Fit:

- 12KB -> 12KB
- 10KB -> 10KB
- 9KB -> 9KB

### 2

Effective Instruction Time: $\frac{10k + n}{k} = 10 + \frac{n}{k}$ (nsec)

### 3

Counter 1: 01101110
Counter 2: 01001001
Counter 3: 00110111
Counter 4: 10001011

## Ex. 2

### Inverted Page Table

An Inverted Page Table is a system-wide data structure used in virtual memory systems. Compared with TLB, the Inverted Page Table contains one entry per physical frame in main memory, storing the paging information for all processes in a single table. This design eliminates the need for individual per-process page tables, and entries are indexed by frame numbers rather than logical page numbers. Using hash function to convert the values, it proves to be more efficient than TLB.
(Source: [GeeksforGeeks: Inverted Page Table in Operating System](https://www.geeksforgeeks.org/operating-systems/inverted-page-table-in-operating-system/))

### Multilevel Page Table

Multilevel Page Table divides the virtual address space into multiple levels of page tables, making memory management more efficient. It reduces memory overhead since each level stores fewer entries, speeds up address translation through quicker lookups, and offers flexibility to adapt to systems with different memory requirements.

(Source: [GeeksforGeeks: Multilevel Paging in Operating System](https://www.geeksforgeeks.org/operating-systems/multilevel-paging-in-operating-system/))

## Ex. 3

Typically there are five common code vulnerabilities listed below:

1. **SQL Injection**

SQL injection occurs when attackers insert malicious SQL statements into input fields or parameters, manipulating database queries, which can lead to unauthorized data access, modification, or deletion.

Example:

Suppose a login form uses this vulnerable code:

```sql
SELECT * FROM users WHERE username = 'admin' AND password = '1234';
```

If the attacker inputs:

```sql
' OR '1'='1
```

as the username, the query then becomes:

```sql
SELECT * FROM users WHERE username = '' OR '1'='1' AND password = '';
```

This can result in the query always returning true, granting unauthorized access to the database.

2. **Cross-Site Scripting**

Cross-Site Scripting injects malicious JavaScript into web pages viewed by others. It runs on users' browsers, stealing cookies, hijacking sessions, or defacing websites.

Three common types:
- Reflected XSS: Malicious script embedded in a URL.
- Stored XSS: Script permanently stored in a database.
- DOM-based XSS: Manipulates page content directly in the browser without server involvement.

Example:

An attacker posts this comment on a webpage:

```html
<script>alert('You have been hacked');</script>
```

Every time another user visits the page, their browser executes the script, so that the attacker could instead steal session tokens or cookies.

3. **Cross-Site Request Forgery**

Cross-Site Request Forgery forces an authenticated user to perform unwanted actions without their consent. It exploits the trust a site has in the user's browser session.

Example:

A user is logged into their bank account. While browsing another website, they may click a malicious link like:

```html
<img src="https://bank.com/transfer?to=attacker&amount=5000" />
```

Then the browser sends the authenticated request automatically, transferring $5000 to the attacker's account.

4. **Out-of-Bounds Write**

Out-of-Bounds Write occurs when a program writes data beyond the memory buffer's limit, overwriting adjacent memory. This can crash systems or let attackers execute arbitrary code.

Example:

```c
char buf[8];
strcpy(buf, "0000000000000");
```

The overflow overwrites memory beyond `buf`, possibly overwriting return addresses, so that attackers could insert malicious machine code here to gain control over the system.

5. **Insecure Open-Source Dependencies**

Using outdated or unverified open-source libraries can introduce known security flaws or malware into the system. Attackers exploit these to gain access before patches are applied.

Example:
A web app uses a vulnerable version of Log4j (the 2021 "Log4Shell" vulnerability). Attackers exploit this by sending malicious input that makes the application execute remote code, allowing them to run arbitrary commands on the server.

(Source: [legitsecurity: Vulnerabilities in Code](https://www.legitsecurity.com/aspm-knowledge-base/vulnerabilities-in-code))

## Ex. 4

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    size_t size_mb = 1024;
    size_t bytes = size_mb * 1024UL * 1024UL;
    size_t step = sysconf(_SC_PAGESIZE);
    char *a = malloc(bytes);
    char *b = malloc(bytes);
    if (!a || !b) { perror("malloc"); return 1; }
    printf("Buffers: %zu MB each; touching every %zu bytes (page size). Ctrl-C to stop.\n",
           size_mb, step);
    for (;;) {
        for (size_t i = 0; i < bytes; i += step) a[i] = 1;
        for (size_t i = 0; i < bytes; i += step) b[i] = 2;
    }
    return 0;
}
```

Also can be found in [ex4.c](src/ex4.c).
