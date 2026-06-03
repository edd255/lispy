# Lispy

> [!NOTE]
> This project is archived, and no longer under active development.

This repository contains an interpreter for a small Lisp-like language called Lispy, based on [*Build Your Own Lisp*](https://buildyourownlisp.com/).
The parser is built using Daniel Holden's [`mpc`](https://github.com/orangeduck/mpc) parser combinator library.
I have added:
* decimal number support
* power, modulo, minimum and maximum operators
* additional logical, list and functional built-ins
* string support in for `head`, `tail` and `join`
* a native implementation of the standard-library operations
* an optional standard library loaded at startup
* a structured command-line argument parser
* modularized source files
* different build targets
* Doxygen-compatible source documentation
* an interactive REPL with command history

Here an example:
```lisp
; arithmetic
(+ 1 2 3)
(^ 10.0 (-1.0))
(max 3 7 2)

; functions and closures
(fn {fac n} {
    select
        { (== n 0) 1 }
        { otherwise (* n (fac (- n 1))) }
})

(def {add} (lambda {x} {lambda {y} {+ x y}}))
((add 2) 3)

; collections and strings
(map (lambda {x} {+ x 10}) {5 2 11})
(filter (lambda {x} {== x "b"}) "abc")
(join "lis" "py")
```

## Repository layout

```
.
|-- .config/        Configuration for make and clang
|-- deps/           Third-party dependencies
|-- lib/            Optional standard library
|-- src/            Implementation
|   |-- builtins/   Built-in functions
|   |-- common/     Environment and values
|   |-- io/         Input/output functions
|   |-- utils/      Argument parsing, allocation and prompt
|-- tests/          Tests
```

## Building

The standard build targets are:

```bash
make optimized  # an optimized release binary
make debugging  # binary with debugging information included
make sanitized  # a binary with sanitizers included, used for testing
make profiling  # generate profile information for prof
```

Additional development targets include:

```bash
make tests      # build the sanitized interpreter and execute test programs
make docs       # generate documentation
make analyze    # run configured static-analysis tools
make memcheck   # run the debugging build under Valgrind
make clean      # clean up all output files
```

## Command-line options

```text
-f, --filename=<path>   Run a .lspy source file
-n, --no-stdlib         Do not load the installed standard library at startup
-w, --print_logs        Write log output to standard error
-h, --help              Show command-line help
```

Without `--filename`, Lispy starts the REPL.
