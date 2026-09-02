# Compiler Hardening Flags and Sanitizer Diagnostics

Program under test: `question1/unsafe.c`

## 1. No flags
Compiles silently. Program runs and prints wrong values (short 4464, unsigned
4294967295, garbage int from the float, clamped atoi result). Nothing warns you.

## 2. -Wall -Wextra
Static warnings only. Flags the constant conversions that lose value
(`-Woverflow` / `-Wconversion`-style notes on the int-to-short and float-to-int
literals). Says nothing about anything computed at runtime, and nothing about
`atoi` silently swallowing an out-of-range string.

## 3. -fsanitize=undefined (UBSan)
Instruments the binary. Catches the float-to-int conversion at runtime:
`runtime error: value 3.9e+09 is outside the range of representable values of
type 'int'`. Does NOT flag int-to-short truncation or signed-to-unsigned
conversion, because both are defined behaviour in C, just not the behaviour the
programmer wanted.

## 4. -fsanitize=address (ASan)
Catches memory errors (buffer overflow, use-after-free, leaks). This program has
none, so ASan reports nothing. Useful reminder that ASan and UBSan cover
different defect classes and are not substitutes.

## Summary

| Defect | none | -Wall -Wextra | UBSan | ASan |
|---|---|---|---|---|
| int -> short truncation | miss | warn | miss | miss |
| signed -> unsigned wrap | miss | miss | miss | miss |
| float -> int out of range | miss | warn | catch | miss |
| bad string -> atoi | miss | miss | miss | miss |

Warnings are cheap and catch mistakes before the program ever runs, but only
where the compiler can see constants. Sanitizers catch real runtime values but
only for behaviour the standard calls undefined. Defined-but-wrong conversions
(truncation, sign change, `atoi` failure) are caught by neither, which is why
the range checks in `question1/safe.c` are still necessary.
