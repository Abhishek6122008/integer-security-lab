#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* defect 2 : memory leak, the block is allocated and never given back */
char *make_message(void)
{
    char *m = malloc(32);
    if (m == NULL)
        return NULL;
    strcpy(m, "hello from the heap");
    return m;
}

int main(void)
{
    char *msg = make_message();
    if (msg == NULL)
        return 1;
    printf("%s\n", msg);
    // free(msg) is missing, so the 32 bytes stay allocated until the
    // process ends and the only pointer to them disappears here
    return 0;
}
