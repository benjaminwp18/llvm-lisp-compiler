# llvm-lisp-compiler
A compiler for a bespoke LISP dialect implemented with LLVM

## TODO
 - [x] Expression sets (`begin`)
 - [ ] Variable declaration/assignment (hoist declarations)
 - [x] If/select expressions
 - [x] Comments (#)
 - [ ] Lists (include common LISP list functions)
 - [ ] Benchmark against other LISP compilers/interpreters
 - [ ] Automatically cast ints to floats on return from function? (line ~156 function.cpp)
 - [ ] Fix double function call bug
