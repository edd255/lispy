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
- [X] Fixing memory bugs
- [X] Fixed my implementation of join, map, split, take, unpack
- [X] Adding tests
- [X] Added support for string for join, head, tail
- [X] Adding builtin logical and list operators
- [X] Adding builtin fun operators
- [X] Writing a function that prints all out named values in an environment

## TODO
However, I'm far from finished.
- [ ] Documenting the code
- [ ] Implementing a builtin function read that reads in and converts a string to a Q-expression
- [ ] Adding functions to wrap all of C's file handling functions such as fopen and fgets
- [ ] Creating a builtin function show that can print the contents of strings as it is (unescaped)
- [ ] Define a recursive Lisp function that returns 1 if an element is a member of a list, otherwise 0
- [ ] Using hash tables for environments
- [ ] Implementing lexical scoping
- [ ] Implementing static typing
- [ ] Implementing list literals
- [ ] Implementing operating system interaction
- [ ] Implementing tail call optimisation
- [ ] Implementing garbage collection
- [ ] Enabling user defined types
