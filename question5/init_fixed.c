#include <stdio.h>
#include <stdlib.h>

/* fix 1 : every variable is given a value before it is read */
int main(void)
{
    int x = 5;                               // initialised at the declaration
    if (x > 10)
        printf("stack x looks big   : %d\n", x);
    else
        printf("stack x looks small : %d\n", x);

    int *heap = malloc(sizeof(int));
    if (heap == NULL)
        return 1;
    *heap = 0;                               // or use calloc, which zeroes for you
    printf("heap value = %d\n", *heap);
    free(heap);
    heap = NULL;
    return 0;
}
