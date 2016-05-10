Compiler from simple C-like language to LLVM IR.
[![Build Status](https://travis-ci.org/yulya3102/compiler.svg?branch=master)](https://travis-ci.org/yulya3102/compiler)

## Build dependencies

* `gcc >= 4.9`
* `boost >= 1.58`
* `flex`
* `bison`
* `libllvm`

## How to build

```
cmake . && make
```

This will build the `lcc` binary in the build directory.

## Language features

* C-like function definitions
* single line comments
* types: `int` (signed 64 bit), `_Bool` (with `true` and `false` values) and `pointer`
* first-class functions (actually, function pointers)
* global variables (default-initialized with `0` or `false`)
* local variables
* arithmetic expessions
* logic expressions
* comparsions
* conditional statement (if-else)
* while loop
* I/O: `read(varname)`, `write(int-expr)`
* optimisations:
    * rewrite recursive function to use accumulator
    * tail call optimisation

## Testing

Tests can be found in `testing/` directory. They embed examples with
described behavior into code, compile them and check if they work as
expected.

## Examples

There are few examples in `examples/` directory:

*   `scope.lc` tests proper scoping for global and local variables
*   `semantic_subscope.lc` tests proper scoping for inner blocks
*   `fact.lc` recursively computes factorial for each number from stdin
    (e.g. `seq 10 | ./fact | head`)
*   `pointer.lc` and `pointer_pointer.lc` show pointer and pointer to
    pointer behavior
*   `func_pointer.lc` shows function pointer behavior
*   `alloca_while` tests if declaring variable inside `while` loop does not
    crash program
*   `arg_var.lc` tests if function arguments can be used as normal variable

Examples can be built with `make` or `make <example name>` in examples
directory. It will produce binaries using `lcc` from
project root directory.  Custom location of `lcc` can be set with `LCC`
environment variable.
