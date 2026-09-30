#include <stdio.h>
#include <stdlib.h>

/* defect 1 : initialisation error, a variable is read before it is written */
int main(void)
{
    int x;                                  // never assigned anything
    if (x > 10)                             // the branch depends on stack junk
        printf("stack x looks big   : %d\n", x);
    else
        printf("stack x looks small : %d\n", x);

    int *heap = malloc(sizeof(int));         // malloc does not clear the memory
    if (heap == NULL)
        return 1;
    printf("heap value = %d\n", *heap);      // read of an uninitialised int
    free(heap);
    return 0;
}
