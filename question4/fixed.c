#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* the same four snippets with the correct pattern applied,
   all four run one after another and the whole program is clean */

// fix 1, check the return value before the pointer is touched
void fixed1(void)
{
    char *p = malloc((size_t)-1);
    if (p == NULL) {                       // this one line is the whole fix
        printf("fix 1 : malloc failed, pointer not used\n");
        return;
    }
    strcpy(p, "hello");
    printf("fix 1 : %s\n", p);
    free(p);
    p = NULL;
}

// fix 2, the pointer is set to NULL as soon as the block is freed
void fixed2(void)
{
    char *p = malloc(16);
    if (p == NULL) return;
    strcpy(p, "hello");
    printf("fix 2 : %s\n", p);             // used while it is still ours
    free(p);
    p = NULL;                              // no dangling pointer is left behind
    if (p == NULL)
        printf("fix 2 : pointer is NULL after free, nothing to read\n");
}

// fix 3, the block is freed exactly once
void fixed3(void)
{
    char *p = malloc(16);
    if (p == NULL) return;
    strcpy(p, "data");
    printf("fix 3 : %s\n", p);
    free(p);
    p = NULL;
    free(p);                               // free(NULL) is defined to do nothing,
    printf("fix 3 : second free was harmless because the pointer was NULL\n");
}

// fix 4, every allocation has a matching free
void fixed4(void)
{
    char *p = malloc(64);
    if (p == NULL) return;
    strcpy(p, "this block is freed properly");
    printf("fix 4 : %s\n", p);
    free(p);
    p = NULL;
}

int main(void)
{
    fixed1();
    fixed2();
    fixed3();
    fixed4();
    printf("all four snippets finished, no errors\n");
    return 0;
}
