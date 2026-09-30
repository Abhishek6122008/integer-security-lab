/* the dirty snippet, every line marked with a number is a finding in the table */

#pragma GCC diagnostic ignored "-Wunused-result"   /* F6, warnings suppressed */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buf[64];                                      /* F1, magic number 64 */

int old_check(char *u)                             /* F2, dead code, never called */
{
    return 1;
}

int check(char *u, char *p)
{
    if (u != 0) {                                  /* F5, five levels of nesting */
        if (strlen(u) > 0) {
            if (p != 0) {
                if (strlen(p) > 0) {
                    if (strcmp(p, "admin123") == 0) {   /* F7, password in source */
                        return 1;
                    } else {
                        return 0;
                    }
                }
            }
        }
    }
    return 0;
}

int main(void)
{
    char name[16];                                 /* F1, magic number 16 */
    char *rec = malloc(64);                        /* F3, return value not checked */

    printf("username: ");
    scanf("%s", name);                             /* F4, unbounded read, like gets */

    strcpy(rec, name);                             /* F4, unbounded copy */

    FILE *f = fopen("log.txt", "a");               /* F3, return value not checked */
    fprintf(f, "%s\n", rec);
    fclose(f);

    if (check(name, "admin123") == 1)
        printf("welcome\n");

    for (int i = 0; i < 64; i++)                   /* F1, magic number 64 again */
        buf[i] = 0;

    return 0;                                      /* F8, rec is never freed */
}
