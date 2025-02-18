# 🐒 Monkey Interpreter

Welcome to the **Monkey Interpreter** project! This repository implements a recursive descent parser for a programming language based on a _top-down operator precedence_ approach, also known as a **Pratt parser**, named after Vaughan Pratt.

The key difference between top-down and bottom-up parsers is that **top-down parsers** construct the root node of the Abstract Syntax Tree (AST) first, then descend, while **bottom-up parsers** start from the leaves and build upwards.

---

## 🚀 Features

- **Top-down parsing:** Implemented using a Pratt parser to handle operator precedence grammars.
- **Recursive Descent:** Efficient parsing for complex expressions.
- **Abstract Syntax Tree (AST):** The interpreter constructs an AST from the parsed input, enabling expression evaluation.

---

## 🛠️ Building the Project

To build and run the Monkey Interpreter, follow these steps:

### 1. Create a Build Directory

Start by creating a `build` directory inside the project directory to store build artifacts:

```bash
mkdir build
cd build
```

### 2. Run CMake

Generate the necessary build files using CMake. If you're using Unix Makefiles, run the following command:

```bash
cmake -G "Unix Makefiles" ..
```

### 3. Build the Project

Once CMake has generated the build files, you can compile the project. Depending on the platform or build system you are using, the command might differ. Below are the typical commands for various systems:

#### For Unix-like Systems (Linux/macOS) using `make`:

```bash
cmake --build . --config Debug -j10 --target monkey
cmake --build . --config Debug -j10 --target monkey_test
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

## NOTES TODOLIST

1. Repl> Hold history, with arrow "up"
2. Repl> typing a wrong string like "lala" causes segmentation fault.
