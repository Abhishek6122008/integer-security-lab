# Memory Defect Detection with Valgrind Memcheck and AddressSanitizer

Three defects, one snippet each, buggy and fixed:

| Defect | Buggy | Fixed |
|---|---|---|
| Initialisation error | `init_buggy.c` | `init_fixed.c` |
| Memory leak | `leak_buggy.c` | `leak_fixed.c` |
| Double free | `doublefree_buggy.c` | `doublefree_fixed.c` |

Valgrind and ASan cannot be used on the same binary, so each snippet is compiled
twice: plain with `-g` for Valgrind, and again with `-fsanitize=address`.

## 1. Initialisation error

`int x;` is read in an `if`, and a malloced int is printed without being written
first. malloc never clears the memory it returns.

**Valgrind:** `Conditional jump or move depends on uninitialised value(s)` for the
`if`, and `Use of uninitialised value of size 4` for the print. Memcheck tracks
one validity bit per byte, so it knows the value was never written and reports the
moment that value changes what the program does.

**ASan:** reports **nothing**. This is the important result of the experiment.
ASan checks *where* you access memory, not *whether the contents were ever set*,
so uninitialised reads are outside what it can see. MemorySanitizer
(`-fsanitize=memory`) is the sanitizer for this class; Valgrind covers it too,
which is why both tools are run rather than one.

gcc also warns at compile time with `-Wmaybe-uninitialized`, which is the cheapest
of the three because it needs no run at all.

**Fix:** `int x = 5;` and `*heap = 0;`, or use calloc, which zeroes for you.

## 2. Memory leak

`make_message` allocates 32 bytes, `main` prints them and returns without freeing,
and the only pointer goes out of scope.

**Valgrind before:**

```
LEAK SUMMARY:
   definitely lost: 32 bytes in 1 blocks
```

*Definitely lost* means no pointer to the block survives anywhere, so the program
could not have freed it even if it wanted to. `--leak-check=full` prints the
allocation stack, naming `make_message` as the line that allocated it.

**Valgrind after:** `All heap blocks were freed -- no leaks are possible`, and
`definitely lost: 0 bytes in 0 blocks`. That pair of lines is the before/after
evidence the experiment asks for.

**ASan:** LeakSanitizer is built into ASan on Linux and reports the same block at
exit: `Direct leak of 32 byte(s) in 1 object(s)`.

**Fix:** `free(msg); msg = NULL;` in the caller.

## 3. Double free

The same address is passed to free twice.

**Valgrind:** `Invalid free() / delete / delete[] / realloc()`, with the stack of
the second free and a second stack headed `Block was alloc.d at`, so both ends of
the mistake are named.

**ASan:** `attempting double-free on 0x... in thread T0`, then three stacks: the
bad free, the first free, and the allocation. ASan's output is the easier of the
two to read here; Valgrind's advantage is that it needs no recompilation.

Why it matters: the allocator stores its own bookkeeping inside the heap, so
freeing a block that is already on the free list corrupts that bookkeeping, and a
corrupted free list can be steered to make a later malloc return an address the
attacker chose. That is what turns a double free from a wasted call into an
exploitable defect.

**Fix:** `p = NULL;` right after the free. `free(NULL)` is defined by the standard
to do nothing, so the second free becomes harmless.

## Summary

| Defect | gcc warnings | Valgrind Memcheck | ASan |
|---|---|---|---|
| Uninitialised read | warns | catches | **misses** |
| Memory leak | miss | catches | catches (LeakSanitizer) |
| Double free | miss | catches | catches |

Neither tool is a superset of the other. Valgrind needs no recompilation and is
the only one of the two that sees uninitialised values; ASan is roughly twenty
times faster and gives clearer stacks. Both are runtime tools, so they only report
defects on the paths actually executed, which is why compiler warnings still
matter first.
