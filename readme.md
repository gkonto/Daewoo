# Daewoo

![C++](https://img.shields.io/badge/C%2B%2B-11%2F14%2F17%2F20%2F23-blue)
![License](https://img.shields.io/github/license/franneck94/CppProjectTemplate)
![Linux Build](https://github.com/franneck94/CppProjectTemplate/workflows/Ubuntu%20CI%20Test/badge.svg)

Daewoo is a custom programming language with a handcrafted bytecode interpreter, written entirely in C++/C. It is designed as a learning-focused, minimalist runtime for exploring internals of programming language execution - from parsing to virtual machine.

---

## 🚀 Features

- **Bytecode Execution Egnine**<br>
A register-based virtual machine capable of executing compiled Daewoo bytecode.
- **Custom Instruction Set**<br>
Tailored instruction set with support for basic operations, control flow, and function calls.
- **Minimal Runtime**<br>
Lightweight and fast, with zero external dependencies.
- **Error Reporting**<br>
Basic diagnostics and runtime error handling to help with debugging.
- **Designed for Extensibility**<br>
AST evaluation, gargabe collection, and advanced types planned in future versions.

---


## Structure


``` text
├── CMakeLists.txt
├── app
│   ├── repl/...
│   └── benchmarks/...
├── cmake
│   └── cmake modules
├── src
│   ├── CMakesLists.txt
│   ├── *.hpp
│   └── *.cpp
└── tests
    ├── CMakeLists.txt
    └── *.t.cpp
```

Library code goes into [src/](src/), main program code in [app/](app) and tests go in [tests/](tests/).



## Software Requirements

- CMake 3.21+
- GNU Makefile
- MSVC 2017 (or higher), G++9 (or higher), Clang++9 (or higher)
- Optional: Code Coverage (only on GNU|Clang): gcovr, clang-tidy, clang-format, cmake-format
- Optional: Makefile


## 🛠️ Building the Project

First, clone this repo and do the preliminary work:

```shell
git clone --recursive https://github.com/gkonto/InterpreterCompiler.git
mkdir build
#or make prepare
```

- REPL Executable

```shell
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target repl
cd app
./main
```

- Unit testing

```shell
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE="Debug"
cmake --build build --config Debug
cd build
ctest .
```

- Code Coverage (Unix only)

```shell
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=On -DENABLE_TESTING=On
cmake --build build --config Debug --target coverage -j4
# open in your browser ./build/coverage/index.html
```

- Running clang-tidy
```shell
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug -DENABLE_CLANG_TIDY=On
cmake --build build --config Debug --target daewoo_clangtidy
```

- Formatting cmake files
```shell
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug -DENABLE_CMAKE_FORMAT=On
cmake --build build --config Debug --target run_cmake_format
```

- Formatting source code
```shell
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug -DENABLE_CLANG_FORMAT=On
cmake --build build --config Debug --target run_clang_format
```

## 📚 Understanding the Pratt Parser

The **Pratt parser** is a powerful top-down parsing technique designed specifically to handle operator precedence efficiently. It allows parsing complex expressions where operators have different precedence levels and associativity rules. The **Monkey Interpreter** uses this method to evaluate expressions in the language.

### How It Works

1. **Parsing Expressions**:
   The Pratt parser works by associating parsing functions with each token type, handling different precedence levels and the structure of expressions. For example, the parser knows how to deal with infix operators like `+` and `*`, respecting their precedence, and it also handles prefix operators like `-` or `!`.

2. **Operator Precedence**:
   In an expression like `2 + 3 * 4`, the Pratt parser ensures that multiplication is evaluated first because `*` has higher precedence than `+`. The resulting parsed expression would be equivalent to `2 + (3 * 4)`.

3. **Associativity**:
   Associativity is handled properly in the Pratt parser. For example, `a - b - c` is evaluated as `(a - b) - c` due to left associativity. Similarly, for exponentiation (if supported), right associativity would ensure the expression `a ^ b ^ c` is parsed as `a ^ (b ^ c)`.

4. **Prefix and Infix Expressions**:
   - **Prefix expressions** (e.g., `-a`, `!b`) are handled by the parser as it descends through the expression tree, applying operators like negation or logical not to the appropriate expressions.
   - **Infix expressions** (e.g., `a + b`, `a * b`) are parsed by identifying the operator precedence and applying the appropriate operations as the parser processes the tokens.

### Example of Parsing

Given the expression: `-5 + 3 * (2 + 1)`, the Pratt parser will:

1. Recognize the prefix operator `-` applied to `5`.
2. Move to `+`, recognize it as an infix operator, and continue parsing the right-hand side.
3. For `3 * (2 + 1)`, it evaluates the `*` with higher precedence, and computes the result of the parenthesized expression `2 + 1` first.
4. Finally, the AST is constructed and evaluated in accordance with operator precedence and associativity.

### Advantages of Using a Pratt Parser

- **Modularity**: New operators with custom precedence can easily be added without refactoring the entire parser.
- **Clarity**: The parser remains concise and easy to understand, even when handling complex grammars.
- **Performance**: The top-down approach ensures that the parser can efficiently process input in a single pass, making it suitable for real-time applications like interpreters.

---

## 📜 License

This project is licensed under the [MIT License](./LICENSE). This means you are free to use, modify, and distribute this project, as long as the terms of the license are respected.

The license guarantees that this project will remain open-source and accessible for all developers. Please refer to the [LICENSE](./LICENSE) file for more detailed legal terms.

---

## 💬 Contact

If you have any questions, suggestions, or feedback, feel free to get in touch:

- **Open an Issue**: If you find a bug or have a feature request, you can report it by opening an issue on the issues page.
- **Email**: You can reach out to us via email at [geor.kontogiannis@gmail.com](mailto:geor.kontogiannis@gmail.com) for any direct inquiries.

We are always happy to engage with users, contributors, or anyone curious about the project!

---

## 🌟 Acknowledgments

TODO

---
