# ECE4820J Homework 3

### Kaiqi Zhu 522370910091

## General questions

1. No, because there is only one thread to duplicate, so no synchronization or I/O inconsistencies between threads can arise.

2. As Windows and UNIX have different APIs and kernel designs, when porting a program from a UNIX system to a Windows system, there might be missing functionality and the program might not run normally.

## C Programming

The code implementation can be found in [Source Directory](src/).

## Research on POSIX

POSIX[^1], namely the Portable Operating System Interface, is a family of standards specified by the IEEE Computer Society for maintaining compatibility between operating systems. It defines APIs, along with command line shells and utility interfaces, for software compatibility with variants of Unix and other operating systems. POSIX is intended to be used by both application and system developers.

The standards emerged from a project that began in 1984 building on work from related activity in the /usr/group association. Richard Stallman suggested the name POSIX to the IEEE instead of the former IEEE-IX. The committee found it more easily pronounceable and memorable, and thus adopted it.

POSIX documentation includes:

- POSIX.1, 2013 Edition: POSIX Base Definitions, System Interfaces, and Commands and Utilities (which include POSIX.1, extensions for POSIX.1, Real-time Services, Threads Interface, Real-time Extensions, Security Interface, Network File Access and Network Process-to-Process Communications, User Portability Extensions, Corrections and Extensions, Protection and Control Utilities and Batch System Utilities. This is POSIX 1003.1-2008 with Technical Corrigendum 1.)
- POSIX Conformance Testing: A test suite for POSIX accompanies the standard: VSX-PCTS or the VSX POSIX Conformance Test Suite.

[^1]: https://zh.wikipedia.org/wiki/Markdown