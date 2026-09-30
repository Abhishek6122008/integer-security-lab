# Defect Table — Memory and Integer Security Audit

Module under audit: `vuln.c`. Remediated module: `fixed.c`.
Each defect is tagged `D1`..`D9` in the comments of both files.

| ID | Defect type | Location in `vuln.c` | Security impact | Caught by |
|---|---|---|---|---|
| D1 | Unchecked allocation return value (CWE-690, CWE-476) | `store_create`, the `malloc(bytes)` result is written to at `s[0].id = 0` before any NULL test | NULL dereference. Denial of service, and a controlled write where the offset is attacker influenced or the zero page can be mapped | ASan |
| D2 | Integer overflow in a size calculation (CWE-190, CWE-680) | `store_create`, `int bytes = count * (int)sizeof(struct record)` | Signed overflow is undefined behaviour, and the wrapped value is then passed to malloc. When it wraps to a small positive number the allocation succeeds but is far too small, and every later write is a heap overflow. This is the classic route from an integer bug to arbitrary code execution | UBSan |
| D3 | Signed to unsigned conversion defeating a bounds check (CWE-195, CWE-787) | `store_set_name`, `if (len > 15)` passes for a negative `len`, then `memcpy(r->name, src, len)` converts it to `size_t` | A negative length becomes roughly 1.8e19 bytes, so the bounds check is bypassed entirely and the copy overruns the destination. Arbitrary code execution | ASan |
| D4 | Truncation of an int into a short used as an index (CWE-197) | `store_touch`, `short idx = id`, then `s[idx % 100]` | 70000 truncates to 4464, and 4464 % 100 is 64, which is far outside a four element store. Out of bounds heap write | ASan, and `-Wconversion` at compile time |
| D5 | Read of uninitialised memory (CWE-457, CWE-908) | `main` case 4, `s[1].id` is read although `store_create` only ever writes `s[0]` | The branch depends on whatever the previous owner of that heap block left behind, so behaviour is unpredictable, and printing it discloses heap contents. Information leak | Valgrind only |
| D6 | Use after free (CWE-416) | `main` case 5, `s[0].id` is read and written after `free(s)` | The freed block is handed to the next allocation, so the stale write corrupts whatever object now lives there, and the stale read leaks its contents. Arbitrary code execution | ASan, Valgrind |
| D7 | Double free (CWE-415) | `main` case 6, `free(s)` twice | Corrupts the allocator's own bookkeeping, which is kept inside the heap. A corrupted free list can be steered so a later malloc returns an attacker chosen address. Arbitrary code execution | ASan, Valgrind |
| D8 | Memory leak (CWE-401) | `main` case 7, the store is never freed | Harmless once, but on a per request path the process grows until the OOM killer takes it. Denial of service | ASan (LeakSanitizer), Valgrind |
| D9 | Unchecked string to integer conversion (CWE-190, CWE-20) | `main` case 8, `atoi("99999999999999")` | atoi has no way to report failure, so an out of range string silently produces a wrong int, which is then used as an allocation count. Whatever the wrong value is, the rest of the program trusts it | none of the tools |

## Remediation summary

| ID | Fix applied in `fixed.c` |
|---|---|
| D1 | Every allocation result is compared with NULL before the pointer is used, and the failure is reported to stderr |
| D2 | `count` is a `size_t` and is checked against `SIZE_MAX / sizeof(struct record)` **before** the multiplication, so the product cannot wrap |
| D3 | `len` is a `size_t`, so it can never be negative, and it is checked against `sizeof r->name` rather than a hand written 15 |
| D4 | The id keeps its own type, nothing is narrowed, and the index is reduced modulo the real store size |
| D5 | `calloc` replaces `malloc`, so every record is zero before it is read |
| D6 | `store_destroy` takes the address of the pointer and sets it to NULL, so no dangling pointer survives the free |
| D7 | The same NULL assignment makes a second `store_destroy` a no-op, because `free(NULL)` is defined to do nothing |
| D8 | Every store has a matching `store_destroy`, including on the error paths |
| D9 | `strtol` with `errno` and an end pointer replaces `atoi`, and the long result is range checked against `INT_MAX` before it is narrowed |

## Tool coverage

| Defect | gcc hardening flags | Valgrind | ASan | UBSan |
|---|---|---|---|---|
| D1 unchecked malloc | miss | catch | catch | miss |
| D2 integer overflow | miss | miss | miss | catch |
| D3 signed conversion | warns (`-Wsign-conversion`) | catch | catch | miss |
| D4 truncation | warns (`-Wconversion`) | catch | catch | miss |
| D5 uninitialised read | miss | catch | **miss** | miss |
| D6 use after free | miss | catch | catch | miss |
| D7 double free | miss | catch | catch | miss |
| D8 memory leak | miss | catch | catch | miss |
| D9 unchecked atoi | miss | miss | miss | miss |

Three results are worth stating plainly. ASan does not detect uninitialised
reads, because it checks *where* memory is accessed and not whether the bytes
were ever written; that class belongs to Valgrind or to MemorySanitizer. UBSan
is the only tool that reports the integer overflow, which is why it is run
alongside ASan rather than instead of it. And D9 is reported by nothing at all,
because `atoi` on an out of range string is a defined library call that simply
returns a wrong answer, which is exactly why the remediated module uses `strtol`.

## Compiler warning comparison (deliverable 4)

Both modules are compiled with the same hardening set:

```sh
gcc -Wall -Wextra -Wconversion -Wsign-conversion -Wformat=2 \
    -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong -c vuln.c
```

`-O2` is included because `_FORTIFY_SOURCE` does nothing without optimisation.
On `vuln.c` this set reports the conversions behind D3 and D4 and the implicit
narrowing in `store_create`; on `fixed.c` it reports nothing, because every
narrowing conversion has either been removed or made explicit after a range
check.
