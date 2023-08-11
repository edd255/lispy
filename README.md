# Lispy
Lispy is a functional language inspired by Lisp. The interpreter is written in C and is based on ["Build Your Own Lisp"](https://buildyourownlisp.com/) by Daniel Holden and uses his [mpc](https://github.com/orangeduck/mpc) library..

## Modifications
So far, I've completed the following bonus tasks, resp. applied the following modifications:
- [X] Power function
- [X] Modulo function
- [X] Support for decimal numbers
- [X] Splitted the code into smaller modules
- [X] Heavily modified the Makefile
- [X] Added an exit function
- [X] Including an (optional) standard library
- [X] Improving the macros to support printing the C function name
- [X] Using a sane argument parser
- [X] Using a logger
- [X] Fixing memory bugs

## TODO
However, I'm far from finished.
- [ ] Documenting the code
- [ ] Adding more tests
- [ ] Writing a function that prints all out named values in an environment
- [ ] Adding builtin logical operators
- [ ] Adapt some builtin functions to work on strings
- [ ] Using hash tables for environments
- [ ] Implementing lexical scoping
