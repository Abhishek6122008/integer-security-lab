#include <stdio.h>
#include <stdlib.h>

/* run with no argument : just the normal malloc -> realloc -> free trace
   run with 1           : error (a), unchecked malloc, NULL dereference
   run with 2           : error (b), use after free through a dangling pointer */
int main(int argc, char **argv)
{
    // stage 1, ask for 5 ints and print where they live and how big the block is
    size_t n = 5;
    int *a = malloc(n * sizeof(int));
    printf("malloc  : ptr = %p  size = %zu bytes\n", (void *)a, n * sizeof(int));
    for (size_t i = 0; i < n; i++)
        a[i] = (int)i * 10;

    // stage 2, grow the same array to 10 ints, realloc is allowed to move the
    // block, so the pointer printed here can be different from the one above
    n = 10;
    a = realloc(a, n * sizeof(int));
    printf("realloc : ptr = %p  size = %zu bytes\n", (void *)a, n * sizeof(int));

    // stage 3, hand the memory back, a keeps the old address and is now dangling
    free(a);
    printf("free    : ptr = %p  size = 0 bytes (block returned to the heap)\n", (void *)a);

    if (argc > 1 && argv[1][0] == '1') {
        // error (a) : this allocation is far too large so malloc returns NULL,
        // the return value is never checked, so the write goes through NULL
        int *big = malloc((size_t)-1);
        printf("huge malloc returned %p\n", (void *)big);
        big[0] = 1;
        printf("this line is never reached\n");
    }

    if (argc > 1 && argv[1][0] == '2') {
        // error (b) : the block was freed above but a still points at it,
        // so both of these touch memory the program no longer owns
        printf("dangling read  : a[0] = %d\n", a[0]);
        a[0] = 99;
        printf("dangling write : done\n");
    }

    return 0;
}
