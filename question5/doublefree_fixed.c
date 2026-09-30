#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* fix 3 : the pointer is set to NULL after the free, so a second free
   is a no-op because free(NULL) is defined to do nothing */
int main(void)
{
    char *p = malloc(16);
    if (p == NULL)
        return 1;
    strcpy(p, "some data");
    printf("%s\n", p);

    free(p);
    p = NULL;                                // the address is dropped here
    free(p);                                 // harmless now
    printf("freed once, second free was a no-op\n");
    return 0;
}
