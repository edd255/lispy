# Lispy
Lispy is a functional language inspired by Lisp. The interpreter is written in C and is based on ["Build Your Own Lisp"](https://buildyourownlisp.com/) by Daniel Holden and uses his [mpc](https://github.com/orangeduck/mpc) library.

## Modifications
So far, I've completed the following bonus tasks, resp. applied the following modifications:
- [x] Adding power, module, min and max operators
- [x] Support for decimal numbers
- [x] Splitted the code into smaller modules
- [x] Heavily modified the Makefile
- [x] Added an exit function
- [x] Including an (optional) standard library at startup
- [x] Improving the macros to support printing the C function name
- [x] Using a sane argument parser
- [x] Using a logging memory allocator
- [x] Using a logger
- [x] Fixing memory bugs in my implementation
- [x] Fixed my implementation of join, map, split, take, unpack
- [x] Adding tests (to some extent)
- [x] Added support for strings in join, head, tail
- [x] Adding builtin logical and list operators
- [x] Adding builtin fun operators
- [x] Writing a function that prints all out named values in an environment
- [x] Documenting the code (to some extent)
- [x] Added length field to strings

## TODO
However, I'm far from finished.
- [ ] Writing a better README.md ;)
- [ ] Using a unit-test framework (that calculates code coverage)
- [ ] Using an integration test framework
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
