#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    // every allocation is checked before it is used, so a NULL return
    // stops the program with a message instead of crashing on a write
    size_t n = 5;
    int *a = malloc(n * sizeof(int));
    if (a == NULL) {
        printf("Error: malloc of %zu bytes failed\n", n * sizeof(int));
        return 1;
    }
    printf("malloc  : ptr = %p  size = %zu bytes\n", (void *)a, n * sizeof(int));

    // fresh memory from malloc holds junk, so it is initialised before use
    for (size_t i = 0; i < n; i++)
        a[i] = (int)i * 10;

    // realloc result goes into a temporary first, if it fails the old block
    // is still valid and can be freed, assigning straight to a would lose it
    size_t m = 10;
    int *tmp = realloc(a, m * sizeof(int));
    if (tmp == NULL) {
        printf("Error: realloc to %zu bytes failed\n", m * sizeof(int));
        free(a);
        return 1;
    }
    a = tmp;
    printf("realloc : ptr = %p  size = %zu bytes\n", (void *)a, m * sizeof(int));

    // realloc does not initialise the part it added, so we do it here
    for (size_t i = n; i < m; i++)
        a[i] = 0;

    // free the block and immediately drop the address, a is no longer dangling
    free(a);
    a = NULL;
    printf("free    : ptr = %p  size = 0 bytes (pointer set to NULL)\n", (void *)a);

    // the same huge request as the unsafe version, but the result is checked
    int *big = malloc((size_t)-1);
    if (big == NULL)
        printf("Error: huge malloc failed, refusing to use the pointer\n");
    else
        free(big);

    // and the same use after free, but the NULL guard stops it
    if (a == NULL)
        printf("pointer is NULL after free, nothing to dereference\n");
    else
        printf("a[0] = %d\n", a[0]);

    return 0;
}
