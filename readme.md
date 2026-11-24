# Atom

Atom is a small, statically typed programming language and a native compiler written in C++20, targeting x86-64 Linux.

## Project Goal

The primary goal of Atom is to explore and understand the internal workings of a compiler, from source code analysis to native executable generation.

The project focuses on implementing the fundamental components of a compiler from scratch, including lexical analysis, parsing, semantic analysis, intermediate representation, optimization, register allocation, and assembly code generation.

Atom is an educational project rather than a production-ready programming language. Its design intentionally favors simplicity, transparency, and a clear separation of compilation stages over language complexity and aggressive optimization.

## Compiler Architecture

The compiler is organized into three major components: the frontend, the middleend, and the backend.

```text
Source Code (.atom)
        |
        v
+---------------------------+
|         Frontend          |
|                           |
|  Lexical Analysis         |
|  Syntax Analysis          |
|  Abstract Syntax Tree     |
|  Semantic Analysis        |
+---------------------------+
        |
        v
+---------------------------+
|         Middleend         |
|                           |
|  IR Generation            |
|  Basic Blocks             |
|  Control Flow Graph       |
|  IR Optimization          |
+---------------------------+
        |
        v
+---------------------------+
|          Backend          |
|                           |
|  Instruction Selection    |
|  Machine IR               |
|  Liveness Analysis        |
|  Register Allocation      |
|  Stack Frame Layout       |
|  Assembly Emission        |
+---------------------------+
        |
        v
x86-64 Assembly (.s)
        |
        v
GNU Assembler (as)
        |
        v
ELF Object File (.o)
        |
        v
GNU Linker (ld)
        |
        v
Native ELF Executable
```

## Frontend

The frontend transforms source code into a semantically validated abstract syntax tree.

- **Lexical Analysis** — converts source code into a sequence of tokens using a handwritten lexer.
- **Syntax Analysis** — uses a handwritten recursive descent parser with precedence climbing for expression parsing.
- **Abstract Syntax Tree (AST)** — represents the hierarchical structure of the program using polymorphic nodes and explicit ownership through smart pointers.
- **Semantic Analysis** — performs name resolution, symbol table management, type checking, mutability validation, function signature validation, and enforcement of language-specific semantic rules.

## Middleend

The middleend transforms the AST into a target-independent intermediate representation and performs basic program optimizations.

- **Intermediate Representation (IR)** — a custom three-address representation based on virtual registers.
- **Basic Blocks** — groups sequences of instructions into straight-line execution units.
- **Control Flow Graph (CFG)** — represents possible execution paths between basic blocks.
- **Constant Folding** — evaluates constant expressions at compile time.
- **Constant Propagation** — propagates known constant values through the intermediate representation.
- **Dead Code Elimination** — removes unnecessary instructions that do not affect observable program behavior.

The intermediate representation is intentionally kept simple and does not use Static Single Assignment (SSA) form.

## Backend

The backend translates the intermediate representation into native x86-64 assembly.

- **Instruction Selection** — maps IR operations to target-specific machine instructions.
- **Machine IR** — represents target-specific instructions and operands before final assembly emission.
- **Liveness Analysis** — determines where virtual register values are live.
- **Register Allocation** — maps virtual registers to physical CPU registers using a linear scan algorithm.
- **Register Spilling** — stores values in stack slots when physical registers are unavailable.
- **Stack Frame Layout** — manages local storage, spill slots, stack alignment, and function prologues and epilogues.
- **Calling Convention** — follows the System V AMD64 ABI for function calls, argument passing, return values, and register preservation.
- **Assembly Emission** — generates textual x86-64 assembly using Intel syntax.

## Assembly and Linking

Atom uses the GNU toolchain for the final stages of executable generation.

- **GNU Assembler (`as`)** — encodes assembly instructions into machine code and produces ELF relocatable object files.
- **GNU Linker (`ld`)** — resolves symbols, applies relocations, and produces native ELF executables.

Machine instruction encoding, ELF serialization, and linking are delegated to external tools, keeping the compiler focused on the fundamental principles of compilation.

## Language Features

Atom provides a minimal, imperative, statically typed language with syntax inspired by Rust and Zig.

- Signed 64-bit integers (`i64`)
- Boolean values (`bool`)
- Explicit type annotations
- Mutable and immutable variable declarations
- Arithmetic and comparison operators
- Conditional statements (`if`, `else`)
- Loops (`while`)
- Functions and function calls
- Recursion
- Explicit return values
- Strict type checking without implicit conversions

### Example

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
    const result: i64 = sum(10);

    if (result > 50)
    {
        return result;
    }

    return 0;
}
```

## Project Structure

```text
Atom/
├── frontend/       # Lexing, parsing, AST, semantic analysis
├── middleend/      # Intermediate representation and optimization
├── backend/        # Register allocation and code generation
├── tests/          # Language and compiler tests
├── grammar.ebnf    # Formal language grammar
├── main.cpp        # Compiler entry point
├── Makefile        # Build system
└── README.md
```

## Technical Overview

| Component | Technology |
|---|---|
| Implementation language | C++20 |
| Target architecture | x86-64 |
| Target operating system | Linux |
| Parsing strategy | Recursive Descent and Precedence Climbing |
| Intermediate representation | Custom Three-Address IR |
| Register allocation | Linear Scan |
| Calling convention | System V AMD64 ABI |
| Assembly syntax | Intel |
| Object file format | ELF |
| Assembler | GNU as |
| Linker | GNU ld |
| Build system | GNU Make |
| Source file extension | `.atom` |
| Compiler executable | `atomc` |

## Design Philosophy

- Implement core compiler components from scratch.
- Avoid external compiler frameworks such as LLVM.
- Avoid lexer and parser generators such as Flex and Bison.
- Maintain explicit and inspectable representations between compilation stages.
- Keep the language small enough to understand the entire compilation pipeline.
- Prioritize correctness, readability, and educational value over production-level performance.

Atom is designed to make the compilation process transparent, providing a practical understanding of how a high-level programming language is transformed into executable machine code.