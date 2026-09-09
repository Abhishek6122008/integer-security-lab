# Dynamic Memory: malloc, realloc, free and the two classic errors

Programs: `question3/unsafe.c` (broken), `question3/safe.c` (corrected).

`unsafe.c` first prints the pointer value and the block size at each stage:

```
malloc  : ptr = 0x502000000010  size = 20 bytes
realloc : ptr = 0x503000000030  size = 40 bytes
free    : ptr = 0x503000000030  size = 0 bytes
```

The pointer usually changes at the realloc, because realloc is allowed to move
the block when it cannot grow it in place. The address printed after free is the
same one as before, which is exactly the problem: freeing changes the heap, not
the variable.

## (a) Unchecked allocation return value

`malloc((size_t)-1)` cannot be satisfied, so malloc returns NULL. The program
never checks it and writes `big[0] = 1`, which dereferences NULL. ASan reports
`SEGV on unknown address 0x000000000000`.

Under ASan a request this large is rejected by the sanitizer itself before the
real allocator sees it, so the run needs `ASAN_OPTIONS=allocator_may_return_null=1`
to make it behave like a normal failing malloc.

**Security impact:** on a normal desktop this is a crash, so a denial of service.
It is worse than it looks, because the crash happens at whatever offset is
written: `p[i] = x` on a NULL `p` with a large attacker controlled `i` writes to
a low address, and on embedded targets or with a mapped zero page that address is
real memory, which turns the crash into a controlled write.

## (b) Use after free through a dangling pointer

After `free(a)` the variable `a` still holds the old address. Reading `a[0]` and
writing `a[0] = 99` both touch memory the program has given back. ASan reports
`heap-use-after-free`, with the line of the bad access, the line of the free, and
the line of the original allocation.

**Security impact:** the freed block is handed out to the next allocation, so the
stale write lands in whatever object now lives there. This is the standard route
to corrupting a struct, a length field, or a function pointer, and use-after-free
is one of the most commonly exploited bug classes in C. The read is a leak of
whatever the new owner put there.

## The corrected version

| Rule | Where it is applied in `safe.c` |
|---|---|
| Check every allocation | `if (a == NULL)` after malloc, and after the huge malloc |
| Never assign realloc to the same pointer | result goes into `tmp`, old block freed only on failure |
| Initialise after allocating | loop fills 0..n, second loop fills the part realloc added |
| NULL the pointer after free | `free(a); a = NULL;` so the dangling read is impossible |

`safe.c` runs clean under ASan: no errors reported, both error paths print a
message instead of crashing.
