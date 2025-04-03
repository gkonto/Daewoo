## NOTES TODOLIST

In book> 3.11 - WHO's TAKING THE TRASH OUT?

TODO things:

1. Repl> Hold history, with arrow "up"
2. I don't like the fact that in order to add Environment on the evaluator,
   i will have to modify the interface of the nodes.
   evaluate function should not exist on the on the API.
   Maybe i need an Evaluator that will evaluate each node based on the type.
3. Exo pantou shared_ptrs. Prepei na ta eksafaniso.
   I na ta kano optimize gia na min anevainei o counter tous. Na to do.
4. Fibonacci.

- Meta apo auta na sinexiso me ton compiler OR me ton 3.11. (protimo compiler arxika)
- Does it compile in windows ?
- Performance in release ????

7.  Add stress tests, both memory and performance related.
8.  TODOs and FIXMEs and comments
9.  UI
10. Mention in the documentation that i have used Catch2 framework to perform unit tests.
    and benchmark!

# BENCHMARKS

## Virtual Machine

...............................................................................

benchmark name samples iterations estimated
mean low mean high mean
std dev low std dev high std dev

---

Fibonacci(33) execution 100 1 2.46345 m
1.47199 s 1.46124 s 1.48487 s
59.6609 ms 49.2897 ms 82.6196 ms

| Build | Name   | samples | iterations | Estimated | mean       | low mean   | high mean  | std dev    | low std dev | high std dev |
| ----- | ------ | ------- | ---------- | --------- | ---------- | ---------- | ---------- | ---------- | ----------- | ------------ |
| rel   | fib 33 | 100     | 1          | 2.46345 m | 1.47199 s  | 1.46124 s  | 1.48487 s  | 59.6609 ms | 49.2897 ms  | 82.6196 ms   |
| rel   | fib 33 | 100     | 1          | 1.084 m   | 751.571 ms | 740.092 ms | 766.516 ms | 66.5698 ms | 53.7242 ms  | 89.0635 ms   |

## AST Evaluation
