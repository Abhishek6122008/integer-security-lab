#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* fix 2 : the caller frees what the function allocated */
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
    free(msg);                               // every malloc has one free
    msg = NULL;
    return 0;
}
