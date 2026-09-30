# Dirty-Code Findings and Standards-Compliant Refactoring

Original: `dirty.c`. Refactored: `clean.c`. Each finding is marked `F1`..`F8` in
the comments of `dirty.c`.

## Findings table

| ID | Practice | Location in `dirty.c` | Security consequence | Standard rule |
|---|---|---|---|---|
| F1 | Magic numbers (64, 16, 64 again) | `char buf[64]`, `char name[16]`, `malloc(64)`, `i < 64` | A buffer size changed in one place and not the others becomes an overflow; the compiler cannot help because nothing ties the numbers together | MISRA C 2012 Dir 4.6; CERT C DCL06-C |
| F2 | Dead code | `old_check()`, never called | Unreachable code is never tested or reviewed, and is a standing risk if a later edit wires it back in | MISRA C 2012 Rule 2.1; CERT C MSC12-C |
| F3 | Unchecked return values | `malloc(64)`, `fopen("log.txt","a")` | malloc returning NULL gives a NULL dereference on the next strcpy; fopen returning NULL gives one inside fprintf. Denial of service, or a controlled write in the bad case | CERT C ERR33-C; MISRA C 2012 Dir 4.7 |
| F4 | Unsafe string functions | `scanf("%s", name)` into a 16 byte buffer, `strcpy(rec, name)` | Classic stack buffer overflow: neither call takes a size, so input longer than the buffer overwrites the saved return address. Arbitrary code execution. gets is the same defect and was removed from the language in C11 | CERT C STR31-C; MISRA C 2012 Rule 21.17; OWASP A03 |
| F5 | Deeply nested logic | `check()`, five levels of `if` | The inner `strlen(p) > 0` test has no else branch, so one path falls through to the outer `return 0` by accident rather than by design. Nesting hides missing cases, and a missing case in an auth check is an auth bypass | MISRA C 2012 Rule 15.7; CERT C structured control flow |
| F6 | Suppressed warnings | `#pragma GCC diagnostic ignored "-Wunused-result"` at the top of the file | Silences exactly the diagnostic that would have reported F3, for the whole file, including code added later. The defect stays, only the message goes away | CERT C MSC00-C (compile cleanly at high warning levels) |
| F7 | Hardcoded credential | `strcmp(p, "admin123")` | The password ships inside the binary, where `strings` finds it in seconds. It is identical on every deployment and cannot be rotated without a rebuild | OWASP A07 Identification and Authentication Failures; CWE-798 |
| F8 | Memory leak | `rec` is never freed | Harmless once, but the same mistake on a per-request path grows the process until the OOM killer takes it. Denial of service | CERT C MEM31-C; MISRA C 2012 Rule 22.1 |

## What the refactor changed

- `NAME_LEN`, `RECORD_LEN` and `LOG_FILE` replace every magic number, and the
  buffer sizes are taken with `sizeof` so they cannot drift apart. (F1)
- `old_check` is deleted. (F2)
- The results of malloc, fgets, fopen and fclose are all tested, and each failure
  path frees what it owns and returns non-zero. (F3, F8)
- `scanf("%s")` becomes `fgets(name, sizeof name, stdin)` plus a `strcspn` call to
  strip the newline; `strcpy` becomes `snprintf(rec, RECORD_LEN, "%s", name)`,
  which always terminates and never writes past the size given. (F4)
- `check()` is flattened into guard clauses with one condition per line, so no
  path falls through by accident, and the parameters are const. (F5)
- The pragma is gone, so the file compiles under `-Wall -Wextra` on its own
  merits. (F6)
- The expected password is read with `getenv("APP_PASS")` instead of being
  compiled in, and a NULL from getenv fails the check closed. (F7)
- The hand written zeroing loop becomes one `memset` with `sizeof buf`. (F1)

## cppcheck

```sh
cppcheck --enable=all --suppress=missingIncludeSystem dirty.c
cppcheck --enable=all --suppress=missingIncludeSystem clean.c
```

`--enable=all` is needed because the interesting findings here are in the style
and unusedFunction categories, not in the default error category. On `dirty.c`
cppcheck reports the unchecked allocation, the leaked `rec`, the unused
`old_check`, the unsafe scanf format and the non-const parameters. On `clean.c`
those are cleared; anything still printed is informational only.

cppcheck is a static analyser, so it reasons about the source without running it.
It finds what it can prove from the text, which is why it catches the dead
function that no runtime tool would ever see, and why it cannot confirm that the
overflow actually happens on a given input.
