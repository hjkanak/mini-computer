# Mini Computer

A small computer built from scratch in C. It has its own programming
language, a compiler, a processor, memory, and a operating system
that runs several programs at once.

## How it works

A program travels through four parts:

1. **Compiler** reads a program written in a simple language and turns it
   into bytecode (a list of numbers).
2. **Memory** stores the bytecode and the program's data.
3. **Processor** repeats three steps: fetch an instruction, decode it,
   execute it.
4. **Operating system** loads programs, gives each one its own memory
   pages, and switches between them so they appear to run together.

## The language

A program is a plain text file with one statement per line.

### Basics

- **Integer registers:** `x0` to `x255`. Each holds a 32-bit number.
- **Vector registers:** `v0` to `v31`. Each holds 8 numbers of 32 bits (256 bits in total).
- **Constants:** whole numbers from 0 to 255.
- **Comments:** everything after a `%` on a line is ignored.
- **Memory:** addressed in bytes. A number takes 4 bytes, so consecutive numbers are 4 addresses apart.

### Assigning a constant

    x1 = 25

Puts the constant into the register.

### Arithmetic

    x1 = x2 + x3
    x1 = x2 - 10
    x1 = x2 * x3
    x1 = x2 / x3

The left side is the destination register. The first operand is always a register. The second operand is either a register or a constant. The four operators are `+`, `-`, `*` and `/`.

### Flags

Every addition and subtraction updates four flags, which conditional jumps use later:

| Flag | Name | Set to 1 when |
|---|---|---|
| Z | Zero | the result is 0 |
| N | Negative | the top bit of the result is 1 |
| C | Carry | addition: the unsigned result is smaller than an input. Subtraction: operand 1 is greater than operand 2 |
| V | Overflow | addition: both inputs have the same sign but the result has a different sign. Subtraction: the inputs have different signs and the result has the same sign as operand 2 |

A common trick is to subtract two registers only to set the flags, and then jump based on them.

### Memory access

Square brackets mean "the memory at this address":

    x1 = [x2]      % read the number at the address held in x2 into x1
    [x2] = x1      % write x1 to the address held in x2
    x1 = [40]      % read the number at address 40
    [40] = x1      % write x1 to address 40

### Labels and jumps

A label marks a position in the program. Rules:

- It starts with `.` and is followed by letters and digits only.
- It must be the very first character on its line (no spaces before it).
- Its line contains nothing else.

A jump moves execution to a label, either always or only when a flag condition is true:

    .loop
    x1 = x1 - 1
    BNE .loop      % jump back to .loop while the result is not zero

A jump is written `B` plus a condition suffix:

| Suffix | Jumps when | Meaning |
|---|---|---|
| `AL` | always | unconditional |
| `EQ` | Z is 1 | equal |
| `NE` | Z is 0 | not equal |
| `CS` | C is 1 | unsigned higher or same |
| `CC` | C is 0 | unsigned lower |
| `MI` | N is 1 | negative |
| `PL` | N is 0 | positive or zero |
| `VS` | V is 1 | overflow |
| `VC` | V is 0 | no overflow |
| `HI` | C is 1 and Z is 0 | unsigned higher |
| `LS` | C is 0 or Z is 1 | unsigned lower or same |
| `GE` | N equals V | signed greater or equal |
| `LT` | N differs from V | signed less than |
| `GT` | Z is 0 and N equals V | signed greater than |
| `LE` | Z is 1 or N differs from V | signed less or equal |

### Vectors

A vector register holds 8 numbers, and one instruction works on all 8 at once.

    v3 = v1 + v2     % v3[i] = v1[i] + v2[i], for i = 0 to 7
    v3 = v1 - 5      % subtract 5 from every element
    v3 = v1 * x4     % multiply every element by the value in x4

Rules:

- Allowed operators are `+`, `-` and `*`.
- The first operand is always a vector register.
- The second operand is a vector register, an integer register or a constant.

Vectors can also be loaded from and stored to memory. This moves 8 consecutive numbers (32 bytes) starting at the given address:

    v1 = [x2]        % load 8 numbers starting at the address in x2
    v1 = [32]        % load 8 numbers starting at address 32
    [x2] = v1        % store the 8 numbers starting at the address in x2

### Printing

    print x1

Writes the register's value, in hex, to a log file along with the id of the process that printed it. This is how a running program shows its results.

### Legacy memory statements

The older forms `Read x1, 0` and `Write x1, 0` (the address is a constant) are still accepted, but the bracket syntax above is preferred.

### Example

Sum an array whose size is stored at address 0 and whose numbers follow from address 4. The result is stored right after the last number.

    x1 = 0           % index
    x2 = 0           % sum
    x3 = 0           % current address
    x4 = [x3]        % read the size of the array
    .loop
    x15 = x1 - x4    % compare index with size
    BEQ .exit        % all numbers added, so stop
    x3 = x3 + 4      % move to the next number
    x6 = [x3]        % load it
    x2 = x2 + x6     % add it to the sum
    x1 = x1 + 1      % count it
    BAL .loop
    .exit
    [x3] = x2        % store the result

## Project layout

    include/         header files for each module
    src/compiler/    text program -> bytecode
    src/cpu/         processor and vector unit
    src/memory/      memory, file I/O, paging
    src/os/          scheduler, shell, loader
    programs/        sample programs
    tests/           tests

## Status

Early development. The project is being built one feature at a time.
