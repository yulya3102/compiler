Compiler from simple C-like language to LLVM IR.

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

TODO

## TODOs

TODO
