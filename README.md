# 4-Bit Sequential Shift-and-Add Multiplier

> A hardware multiplier implemented in VHDL and ported to C, C++, and C# for cross-platform simulation and educational use.

---

## Overview

This repository implements a **4-bit sequential shift-and-add multiplier** — a fundamental digital circuit that computes the product of two 4-bit unsigned integers and produces an 8-bit result.

The design originates as a synthesizable **VHDL** hardware description and is faithfully re-implemented in **C**, **C++**, and **C#** to demonstrate how the same algorithm behaves across hardware description and software programming languages.

---

## Algorithm — Shift-and-Add

The multiplier uses the classic **shift-and-add** method:

```
For each bit of Q (LSB to MSB):
  If Q[0] == 1  →  A = A + M        (add phase)
  Shift right: {E, A, Q} >> 1       (shift phase)
  Repeat for 4 cycles
Result: {A, Q} = 8-bit product
```

Each multiplication takes **4 iterations** (8 clock cycles in hardware — one for add, one for shift per bit).

---

## Repository Structure

```
sequential-multiplier/
│
├── vhdl/
│   └── multi.vhd          # Original VHDL hardware description
│
├── c/
│   └── multi.c            # C simulation (procedural)
│
├── cpp/
│   └── multi.cpp          # C++ simulation (OOP with class)
│
├── csharp/
│   └── Multiplier.cs      # C# simulation (.NET 6+, OOP)
│
└── README.md
```

---

## VHDL — Entity Ports

| Port  | Direction | Width  | Description             |
|-------|-----------|--------|-------------------------|
| `clk` | IN        | 1-bit  | Clock signal            |
| `res` | IN        | 1-bit  | Active-low reset        |
| `M`   | IN        | 4-bit  | Multiplicand            |
| `Q`   | IN        | 4-bit  | Multiplier              |
| `prod`| OUT       | 8-bit  | Product = M × Q         |

### Internal Signals

| Signal  | Width  | Role                                  |
|---------|--------|---------------------------------------|
| `A`     | 4-bit  | Accumulator (partial product)         |
| `E`     | 1-bit  | Extension bit (carry out of A)        |
| `Q_reg` | 4-bit  | Shifting copy of the multiplier       |
| `sum`   | 5-bit  | Combinational: `A + M`                |
| `flag`  | 1-bit  | Alternates between add and shift phase|
| `count` | int    | Counts down from 4 to 0               |

---

## Getting Started

### VHDL (simulation with GHDL)

```bash
cd vhdl
ghdl -a multi.vhd
ghdl -e multi
```

Or open in **Vivado**, **ModelSim**, or **Quartus Prime**.

### C

```bash
cd c
gcc -Wall -O2 -o multi multi.c
./multi
```

### C++

```bash
cd cpp
g++ -std=c++17 -Wall -O2 -o multi multi.cpp
./multi
```

### C# (.NET 6+)

```bash
cd csharp
dotnet new console -o MultiplierApp --force
cp Multiplier.cs MultiplierApp/Program.cs
cd MultiplierApp
dotnet run
```

---

## Sample Output

```
=== 4-bit Sequential Shift-and-Add Multiplier ===

   M     Q    Expected      Got    Status
---------------------------------------------
   3     4          12       12    PASS
   5     6          30       30    PASS
   7     3          21       21    PASS
  15    15         225      225    PASS
   0     9           0        0    PASS
   1    15          15       15    PASS
   6    11          66       66    PASS
   9    14         126      126    PASS
---------------------------------------------
Results: 8 PASS / 0 FAIL
```

---

## Signal Timing Diagram (one iteration, M=3, Q=5)

```
Clock:  ___|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_
Reset:  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
flag:     1   0   1   0   1   0   1   0
phase:   ADD SHF ADD SHF ADD SHF ADD SHF
count:    4   4   4   3   3   2   2   1   1→0
```

---

## Educational Notes

- **VHDL** models the actual hardware register transfers on each clock edge.
- **C** uses a plain struct and function calls to mirror those transfers procedurally.
- **C++** wraps the design into a class, making instantiation and reuse natural.
- **C#** adds a `record`-based test infrastructure and idiomatic .NET style.

All four implementations produce **identical results** for any valid 4-bit input pair.

---

## Limitations

- Inputs are **unsigned 4-bit** values only (0–15).
- The signed / two's complement case is not handled.
- VHDL version uses `std_logic_unsigned` — consider migrating to `numeric_std` for strict IEEE compliance.

---

## License

MIT License. Free to use for educational and research purposes.

---

## Author

*Replace this section with your name, university, or course information.*
