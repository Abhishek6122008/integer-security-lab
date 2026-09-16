# Memory Management Errors: Identification and Remediation

Programs: `question4/original.c` (four buggy snippets), `question4/fixed.c` (the
same four, corrected). Each buggy snippet is selected by a command line argument,
because ASan stops the program at the first error it finds.

## Classified errors

| # | Error type | CWE | What ASan reports | Security impact |
|---|---|---|---|---|
| 1 | Unchecked allocation return value, NULL pointer dereference | CWE-690 / CWE-476 | `SEGV on unknown address 0x000000000000` | Denial of service; a controlled write if the offset is attacker controlled or the zero page is mappable |
| 2 | Use after free through a dangling pointer | CWE-416 | `heap-use-after-free`, READ then WRITE of size 1 | Arbitrary code execution; the freed block is reused by another object, so the stale write corrupts it. The stale read is an information leak |
| 3 | Double free | CWE-415 | `attempting double-free` | Heap metadata corruption leading to arbitrary code execution; historically the classic heap exploit primitive |
| 4 | Memory leak | CWE-401 | `detected memory leaks`, 64 bytes in 1 object | Denial of service; in a long running server the process grows until it is killed or the machine swaps |

## Snippet by snippet

**1. Unchecked return value.** `malloc((size_t)-1)` cannot be satisfied, so it
returns NULL. malloc's only way of reporting failure is that return value, and
the snippet ignores it, so `strcpy(p, "hello")` writes to address 0. The fix is
one `if (p == NULL)` before the pointer is touched.

**2. Use after free.** `free(p)` returns the block to the heap but leaves `p`
holding the old address, because C passes the pointer by value and free cannot
change the caller's variable. Both the `printf` read and the second `strcpy`
touch memory the program no longer owns. The fix is `p = NULL;` immediately after
the free, which turns any later use into an obvious crash instead of silent
corruption.

**3. Double free.** The same address is passed to free twice. The allocator keeps
its bookkeeping in the heap itself, so freeing a block that is already on the
free list corrupts that bookkeeping, which is what makes double free exploitable
rather than merely wrong. The fix is the same one line: once `p` is NULL the
second `free(p)` is a no-op, because the standard defines `free(NULL)` as doing
nothing.

**4. Memory leak.** The 64 byte block is never freed and the only pointer to it
goes out of scope when the function returns, so the memory cannot be reclaimed.
LeakSanitizer, which ships inside ASan, reports it at exit with the allocation
stack. The fix is a matching `free(p)`.

## The pattern

Three rules cover all four snippets:

1. Check the return value of every allocation before using the pointer.
2. Set the pointer to NULL immediately after freeing it.
3. Free exactly once; with rule 2 applied, an accidental second free is harmless.

`fixed.c` applies all three and runs all four snippets in one process. ASan
reports nothing: no errors, no leaks.
