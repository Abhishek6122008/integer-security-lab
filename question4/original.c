#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* the four snippets, each one with a single memory management error
   run with 1 : return value of malloc is never checked
   run with 2 : use after free
   run with 3 : double free
   run with 4 : memory leak                                          */

// snippet 1, the pointer is used straight away without checking it
void snippet1(void)
{
    char *p = malloc((size_t)-1);          // far too big, malloc returns NULL
    printf("snippet 1 : malloc returned %p\n", (void *)p);
    strcpy(p, "hello");                    // writing through a NULL pointer
    printf("snippet 1 : %s\n", p);
    free(p);
}

// snippet 2, the block is given back and then used again
void snippet2(void)
{
    char *p = malloc(16);
    strcpy(p, "hello");
    free(p);                               // block goes back to the heap
    printf("snippet 2 : %s\n", p);         // read of memory we no longer own
    strcpy(p, "world");                    // write into that same memory
    printf("snippet 2 : %s\n", p);
}

// snippet 3, the same address is handed back twice
void snippet3(void)
{
    char *p = malloc(16);
    strcpy(p, "data");
    printf("snippet 3 : %s\n", p);
    free(p);
    free(p);                               // second free of the same block
    printf("snippet 3 : freed twice\n");
}

// snippet 4, the block is never given back
void snippet4(void)
{
    char *p = malloc(64);
    strcpy(p, "this block is never freed");
    printf("snippet 4 : %s\n", p);
    // free(p) is missing, the 64 bytes stay allocated until the program ends
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s 1|2|3|4\n", argv[0]);
        return 1;
    }
    switch (argv[1][0]) {
        case '1': snippet1(); break;
        case '2': snippet2(); break;
        case '3': snippet3(); break;
        case '4': snippet4(); break;
        default : printf("pick 1, 2, 3 or 4\n"); return 1;
    }
    return 0;
}
