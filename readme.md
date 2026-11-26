# Atom

Atom is a statically and strongly typed, compiled programming language with a minimal syntax inspired by Rust and Zig.

The Atom compiler (`atomc`) is written in C++20 and targets the x86-64 architecture on Linux. It translates source code into native machine code, producing standalone ELF executables without requiring a virtual machine, interpreter, or external runtime.

## Language Features

- Static and strong typing
- Explicit type annotations
- Mutable and immutable variables
- Integer and Boolean data types
- Arithmetic and comparison operators
- Conditional statements (`if`, `else`)
- Loops (`while`)
- Functions and recursion
- Explicit return types
- Native x86-64 code generation
- Compile-time semantic analysis
- Intermediate representation optimization

## Type System

Atom provides two primitive data types:

| Type | Description |
|---|---|
| `i64` | Signed 64-bit integer |
| `bool` | Boolean value (`true` or `false`) |

All variable types, function parameter types, and return types must be explicitly declared.

Implicit type conversions are not supported. Type compatibility is checked at compile time.

## Syntax

### Variables

Mutable variables are declared using `mut`, while immutable variables are declared using `const`.

```rust
mut x: i64 = 10;
const y: i64 = 20;

x = x + y;
```

### Functions

Functions are declared using the `fn` keyword.

```rust
fn sum(a: i64, b: i64) -> i64
{
    return a + b;
}
```

### Conditional Statements

```rust
if (x > 10)
{
    x = x - 1;
}
else
{
    x = x + 1;
}
```

### Loops

```rust
while (x > 0)
{
    x = x - 1;
}
```

### Example Program

```rust
fn sum(n: i64) -> i64
{
    mut i: i64 = n;
    mut result: i64 = 0;

    while (i > 0)
    {
        result = result + i;
        i = i - 1;
    }

    return result;
}

fn main() -> i64
{
    const x: i64 = sum(10);
    const success: bool = x > 50;

    if (success)
    {
        return x;
    }

    return 0;
}
```

Every Atom program must define a `main` function with the following signature:

```rust
fn main() -> i64
```

## Compiler Architecture

The Atom compiler is divided into three main components: Frontend, Middleend, and Backend.

### Compilation Pipeline

```text
Source Code (.atom)
        |
        v
      Lexer
        |
        v
      Parser
        |
        v
       AST
        |
        v
Semantic Analysis
        |
        v
  IR Generation
        |
        v
 IR Optimization
        |
        v
 Code Generation
        |
        v
 Assembly (.s)
        |
        v
 GNU Assembler (as)
        |
        v
 Object File (.o)
        |
        v
 GNU Linker (ld)
        |
        v
 ELF Executable
```

### Frontend

The Frontend processes source code and verifies its syntactic and semantic correctness.

- **Lexer** — converts source code into a sequence of tokens.
- **Parser** — constructs an Abstract Syntax Tree using recursive descent parsing and precedence climbing.
- **AST** — represents the syntactic structure of the program.
- **Semantic Analyzer** — performs name resolution, type checking, mutability checking, function signature validation, and entry point validation.

The Frontend is implemented without external lexer or parser generators.

### Middleend

The Middleend translates the AST into a custom Intermediate Representation (IR) and performs optimizations.

The IR uses a three-address instruction format with virtual registers, memory slots, and basic blocks.

Its main components include:

- IR Generator
- IR Verifier
- Basic Blocks
- Control Flow Graph (CFG)
- Constant Folding
- Constant Propagation
- Dead Code Elimination
- Unreachable Block Elimination

Optimization passes are executed repeatedly until no further changes are detected.

### Backend

The Backend translates optimized IR instructions into x86-64 assembly using Intel syntax.

Its responsibilities include:

- Stack frame layout
- Function prologue and epilogue generation
- Virtual register and memory slot mapping
- Arithmetic and comparison instruction generation
- Conditional and unconditional jumps
- Function calls and return values
- Argument passing according to the System V AMD64 ABI
- Program entry point generation

Virtual registers and local variables are stored in stack frames, while physical registers are used to perform operations and transfer values.

The Backend generates a custom `_start` entry point, allowing compiled programs to execute without the C standard library.

### Assembler and Linker

Atom uses GNU Binutils to produce native executables.

- **GNU `as`** translates assembly instructions into machine code and generates relocatable ELF object files.
- **GNU `ld`** links object files and produces executable ELF binaries.

### Compiler Driver

The Compiler Driver manages the final compilation stages, including:

- Output file management
- Assembler execution
- Linker execution
- External process error handling
- Intermediate file cleanup

The assembler and linker are executed directly through Linux process management interfaces.

## Project Structure

```text
Atom/
├── frontend/
│   ├── token.hpp
│   ├── token.cpp
│   ├── lexer.hpp
│   ├── lexer.cpp
│   ├── ast.hpp
│   ├── ast_printer.hpp
│   ├── ast_printer.cpp
│   ├── parser.hpp
│   ├── parser.cpp
│   ├── semantic.hpp
│   └── semantic.cpp
├── middleend/
│   ├── ir.hpp
│   ├── ir.cpp
│   ├── ir_generator.hpp
│   ├── ir_generator.cpp
│   ├── ir_printer.hpp
│   ├── ir_printer.cpp
│   ├── ir_verifier.hpp
│   ├── ir_verifier.cpp
│   ├── cfg.hpp
│   ├── cfg.cpp
│   ├── cfg_printer.hpp
│   ├── cfg_printer.cpp
│   ├── constant_folding.hpp
│   ├── constant_folding.cpp
│   ├── constant_propagation.hpp
│   ├── constant_propagation.cpp
│   ├── dead_code_elimination.hpp
│   └── dead_code_elimination.cpp
├── backend/
│   ├── code_generator.hpp
│   └── code_generator.cpp
├── tests/
├── bin/
├── driver.hpp
├── driver.cpp
├── main.cpp
├── grammar.ebnf
├── Makefile
└── README.md
```

The `bin/` directory is created automatically during compilation.

## Requirements

- Linux (x86-64)
- C++20-compatible compiler
- GNU Make
- GNU Binutils (`as` and `ld`)

### Installing Dependencies

On Fedora:

```bash
sudo dnf install gcc-c++ make binutils
```

On Ubuntu or Debian:

```bash
sudo apt install g++ make binutils
```

## Building

From the project root directory, run:

```bash
make
```

This creates the `atomc` compiler executable in the project root.

## Usage

Atom source files use the `.atom` extension.

Create a file named `example.atom` in the project root directory:

```rust
fn main() -> i64
{
    return 42;
}
```

Compile the program:

```bash
./atomc example.atom
```

The compiler automatically generates an executable inside the `bin/` directory:

```text
bin/example
```

Run the compiled program:

```bash
./bin/example
```

Inspect its exit status:

```bash
echo $?
```

Expected result:

```text
42
```

### Compiling Files from Other Directories

Source files are not required to be located in the project root.

Relative and absolute paths are supported:

```bash
./atomc tests/backend.atom
```

```bash
./atomc /path/to/program.atom
```

Output executables are generated in the `bin/` directory relative to the current working directory.

The executable name is derived from the source filename without its extension.

For example:

```text
tests/backend.atom → bin/backend
/path/to/program.atom → bin/program
```

Intermediate assembly and object files are automatically removed after successful compilation.

## Testing

Run the compiler test suite:

```bash
make test
```

The test suite covers lexical analysis, parsing, semantic validation, control flow, IR generation, and optimization.

## Cleaning

Remove build artifacts and generated executables:

```bash
make clean
```

Rebuild the compiler:

```bash
make clean
make
```

## Current Language Scope

Atom currently supports a deliberately minimal set of language constructs:

- Two primitive data types: `i64` and `bool`
- Up to two parameters per function
- Function-level variable scope
- Immutable function parameters
- Explicit variable and return types
- Arithmetic operators: `+`, `-`, `*`
- Comparison operators: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Unary negation
- Line comments using `//`

Strings, floating-point numbers, arrays, pointers, structures, dynamic memory allocation, and standard library functionality are not currently supported.

Programs communicate their result through the process exit status. Linux exposes the lowest eight bits of the returned value as the exit status.

## Implementation

| Component | Technology |
|---|---|
| Implementation language | C++20 |
| Target architecture | x86-64 |
| Target operating system | Linux |
| Parser | Recursive descent |
| Expression parsing | Precedence climbing |
| Type system | Static, strong |
| Intermediate representation | Custom three-address IR |
| Assembly syntax | Intel |
| Calling convention | System V AMD64 ABI |
| Assembler | GNU `as` |
| Linker | GNU `ld` |
| Executable format | ELF64 |
| Build system | GNU Make |
| External runtime | None |