# ECE4820J Lab 5

### Kaiqi Zhu 522370910091

## Layer programming

There are three layers, core and data layer, IO layer, and interface layer.

The code is shown in [the src directory](/src/).

## Libraries

### Four stages of compiling a file

- Preprocessing: Handles all #include, #define, and conditional compilation directives. Produces a pure C file with everything expanded.

- Compilation: Translates C code into assembly language for the target CPU.

- Assembly: Converts the assembly code into machine code.

- Linking: Links object files and libraries into a single executable binary, resolving all function references.

### The difference between a library and the API

The library is a collection of pre-written code that can be reuse instead of writing everything from scratch, while the API is the contract or specification that describes how your code interacts with a library, a system, or a service.

API implementation is in [the related code](/src/lab5_dlist.c).
