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
* types: `int` (signed 64 bit) and `_Bool` (with `true` and `false` values)
* global variables (default-initialized with `0` or `false`)
* local variables
* arithmetic expessions
* logic expressions
* comparsions
* conditional statement (if-else)
* while loop
* I/O: `read(varname)`, `write(int-expr)`

## Examples

There are two examples in `examples/` directory:

*   `scope.lc` tests proper scoping for global and local variables
*   `fact.lc` recursively computes factorial for each number from stdin
    (e.g. `seq 10 | ./fact | head`)

Examples can be built with `make` or `make <example name>` in examples
directory. It will produce `scope` and `fact` binaries with `lcc` from
project root directory.  Custom location of `lcc` can be set with `LCC`
environment variable.

## TODOs

* I/O error handling
* pointer types
