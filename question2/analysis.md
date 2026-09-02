# Compiler Hardening Flags and Sanitizer Diagnostics

Program under test: `question1/unsafe.c`

Two things had to be arranged for the flags to have anything to report. The
conversion sources are `volatile` so gcc cannot fold them at compile time and
skip instrumenting them, and the truncated short is used as an array index into
a 10 byte heap buffer so there is a real memory error for ASan to find.

## 1. No flags
Compiles silently. Prints wrong values (short 4464, unsigned 4294967295, int
-2147483648 from the float, 276447231 from atoi) and writes past the end of the
buffer without complaint. Nothing warns you.

## 2. -Wall -Wextra -Wconversion
Static warnings only, before the program ever runs. `-Wall -Wextra` alone say
nothing here, because implicit conversions are legal C. `-Wconversion` is the
flag that reports them: conversion from `int` to `short int` may change value,
and conversion from `float` to `int` may change value. It cannot see the atoi
result or the buffer write, because both depend on runtime values.

## 3. -fsanitize=undefined (UBSan)
Instruments the binary and checks at runtime. Catches the float to int
conversion: `runtime error: value 3.9e+09 is outside the range of representable
values of type 'int'`. Does not flag int to short truncation or signed to
unsigned conversion, because both are defined behaviour in C, just not the
behaviour that was wanted. It also does not catch the heap write.

## 4. -fsanitize=address (ASan)
Catches memory errors at runtime. Reports
`heap-buffer-overflow ... WRITE of size 1` on `buf[s % 100]`, with the line
number of the write and of the malloc. It says nothing about any of the
conversions, which is why ASan and UBSan are used together, not as substitutes.

## Summary

| Defect | none | -Wall -Wextra | +-Wconversion | UBSan | ASan |
|---|---|---|---|---|---|
| int -> short truncation | miss | miss | warn | miss | miss |
| signed -> unsigned wrap | miss | miss | miss | miss | miss |
| float -> int out of range | miss | miss | warn | catch | miss |
| bad string -> atoi | miss | miss | miss | miss | miss |
| heap write past buffer | miss | miss | miss | miss | catch |

Warnings are cheap and catch mistakes before the program is ever run, but only
where the compiler can prove the value is wrong. Sanitizers see the real runtime
values but each covers one class of defect: UBSan undefined behaviour, ASan
memory errors. Signed to unsigned conversion and a failed atoi are caught by
none of the four, because both are perfectly legal C, which is why the explicit
range checks in `question1/safe.c` are still necessary.
