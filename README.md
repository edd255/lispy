# Lispy
Lispy is a functional language inspired by Lisp. The interpreter is written in C and is based on ["Build Your Own Lisp"](https://buildyourownlisp.com/) by Daniel Holden and uses his [mpc](https://github.com/orangeduck/mpc) library.

## Modifications
So far, I've completed the following bonus tasks, resp. applied the following modifications:
- [X] Adding power, module, min and max operators
- [X] Support for decimal numbers
- [X] Splitted the code into smaller modules
- [X] Heavily modified the Makefile
- [X] Added an exit function
- [X] Including an (optional) standard library at startup
- [X] Improving the macros to support printing the C function name
- [X] Using a sane argument parser
- [X] Using a logging memory allocator
- [X] Using a logger
- [X] Fixing memory bugs in my implementation
- [X] Fixed my implementation of join, map, split, take, unpack
- [X] Adding tests (to some extent)
- [X] Added support for strings in join, head, tail
- [X] Adding builtin logical and list operators
- [X] Adding builtin fun operators
- [X] Writing a function that prints all out named values in an environment
- [X] Documenting the code (to some extent)

## TODO
However, I'm far from finished.
- [ ] Implementing all stdlib methods in C
- [ ] Adding functions to wrap all of C's file handling functions such as fopen and fgets
- [ ] Using hash tables for environments
- [ ] Implementing lexical scoping
- [ ] Implementing static typing
- [ ] Implementing list literals
- [ ] Implementing operating system interaction
- [ ] Implementing tail call optimisation
- [ ] Implementing garbage collection
- [ ] Enabling user defined types
