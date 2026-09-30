#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* defect 3 : double free, the same address is handed back twice */
int main(void)
{
    char *p = malloc(16);
    if (p == NULL)
        return 1;
    strcpy(p, "some data");
    printf("%s\n", p);

    free(p);                                 // correct, the block goes back
    free(p);                                 // wrong, p still holds the old
                                             // address and this frees it again
    printf("freed twice\n");
    return 0;
}
