#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* the module under audit, a small store of fixed size records
   every defect is tagged D1..D9 and listed in DEFECTS.md

   run with 1 : integer overflow in the size calculation, unchecked malloc
   run with 2 : signed length defeats the bounds check
   run with 3 : truncation of an int into a short used as an index
   run with 4 : read of uninitialised memory
   run with 5 : use after free
   run with 6 : double free
   run with 7 : memory leak
   run with 8 : unchecked string to integer conversion               */

#define STORE_SIZE 4

struct record {
    int  id;
    char name[16];
};

/* D1 : the result of malloc is never checked
   D2 : count is multiplied by the element size in int arithmetic, so a large
        count overflows before malloc ever sees the number */
struct record *store_create(int count)
{
    int bytes = count * (int)sizeof(struct record);
    struct record *s = malloc(bytes);
    printf("store_create : count=%d bytes=%d ptr=%p\n", count, bytes, (void *)s);
    s[0].id = 0;                        /* written before any NULL check */
    return s;
}

/* D3 : len is a signed int, so the bounds check passes for a negative value,
        and memcpy then converts that negative number to a huge size_t */
void store_set_name(struct record *r, const char *src, int len)
{
    if (len > 15)                       /* looks like a bounds check */
        return;
    memcpy(r->name, src, len);
    r->name[15] = '\0';
}

/* D4 : the id is truncated into a short and the truncated value is then
        used to index the store */
void store_touch(struct record *s, int id)
{
    short idx = id;
    printf("store_touch  : id=%d truncated to %d\n", id, idx);
    s[idx % 100].id = idx;
}

int main(int argc, char **argv)
{
    struct record *s;

    if (argc < 2) {
        printf("usage: %s 1..8\n", argv[0]);
        return 1;
    }

    switch (argv[1][0]) {

    case '1':                           /* D1 and D2 */
        s = store_create(200000000);    /* 200000000 * 20 does not fit in an int */
        free(s);
        break;

    case '2':                           /* D3 */
        s = store_create(STORE_SIZE);
        store_set_name(&s[0], "hello", -1);
        free(s);
        break;

    case '3':                           /* D4 */
        s = store_create(STORE_SIZE);
        store_touch(s, 70000);          /* 70000 becomes 4464 in a short */
        free(s);
        break;

    case '4':                           /* D5, malloc does not clear memory */
        s = store_create(STORE_SIZE);
        if (s[1].id > 0)
            printf("record 1 looks used, id = %d\n", s[1].id);
        else
            printf("record 1 looks free, id = %d\n", s[1].id);
        free(s);
        break;

    case '5':                           /* D6 */
        s = store_create(STORE_SIZE);
        free(s);
        printf("after free   : id = %d\n", s[0].id);
        s[0].id = 99;
        break;

    case '6':                           /* D7 */
        s = store_create(STORE_SIZE);
        free(s);
        free(s);
        break;

    case '7':                           /* D8, the store is never freed */
        s = store_create(STORE_SIZE);
        store_set_name(&s[0], "leaked", 6);
        printf("record 0 name = %s\n", s[0].name);
        break;

    case '8': {                         /* D9 */
        const char *text = "99999999999999";
        int n = atoi(text);             /* atoi cannot report failure */
        printf("atoi(\"%s\") = %d\n", text, n);
        s = store_create(n);
        free(s);
        break;
    }

    default:
        printf("pick 1 to 8\n");
        return 1;
    }

    return 0;
}
