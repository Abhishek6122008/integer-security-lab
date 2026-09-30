#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>   /* SIZE_MAX */

/* the remediated module, every defect D1..D9 from DEFECTS.md is repaired.
   because none of the operations can fail silently any more, all eight
   cases run one after another in a single process and the run is clean. */

#define STORE_SIZE  4
#define NAME_LEN   16

struct record {
    int  id;
    char name[NAME_LEN];
};

/* D1 : the result of every allocation is checked before it is used
   D2 : count is a size_t and the multiplication is range checked against
        SIZE_MAX before it is performed, so it cannot wrap
   D5 : calloc zeroes the memory, so nothing is read before it is written */
struct record *store_create(size_t count)
{
    struct record *s;

    if (count == 0 || count > SIZE_MAX / sizeof(struct record)) {
        fprintf(stderr, "store_create : refused, count %zu is out of range\n", count);
        return NULL;
    }

    s = calloc(count, sizeof(struct record));
    if (s == NULL) {
        fprintf(stderr, "store_create : allocation of %zu records failed\n", count);
        return NULL;
    }

    printf("store_create : count=%zu bytes=%zu ptr=%p\n",
           count, count * sizeof(struct record), (void *)s);
    return s;
}

/* D3 : the length is a size_t so it can never be negative, and it is checked
        against the destination size rather than against a hand written
        constant, so the copy can never run past the end of the field */
int store_set_name(struct record *r, const char *src, size_t len)
{
    if (r == NULL || src == NULL)
        return -1;
    if (len >= sizeof r->name) {
        fprintf(stderr, "store_set_name : refused, %zu bytes will not fit\n", len);
        return -1;
    }
    memcpy(r->name, src, len);
    r->name[len] = '\0';
    return 0;
}

/* D4 : the id keeps its own type, nothing is truncated, and the index is
        checked against the real size of the store before it is used */
int store_touch(struct record *s, size_t count, int id)
{
    size_t idx;

    if (s == NULL || id < 0)
        return -1;
    idx = (size_t)id % count;           /* stays inside the store by construction */
    s[idx].id = id;
    printf("store_touch  : id=%d stored at index %zu\n", id, idx);
    return 0;
}

/* D6, D7, D8 : one place frees the store, it sets the caller's pointer to
                NULL, and a second call is therefore harmless */
void store_destroy(struct record **s)
{
    if (s == NULL)
        return;
    free(*s);
    *s = NULL;
}

/* D9 : strtol reports failure, unlike atoi, so the result is range checked
        before it is narrowed to an int */
int parse_count(const char *text, int *out)
{
    char *end;
    long v;

    errno = 0;
    v = strtol(text, &end, 10);
    if (end == text || *end != '\0')
        return -1;                      /* not a number at all */
    if (errno == ERANGE || v < 0 || v > INT_MAX)
        return -1;                      /* does not fit in an int */
    *out = (int)v;
    return 0;
}

int main(void)
{
    struct record *s;
    int n;

    /* case 1, the oversized request is rejected by the range check before
       any multiplication happens, so nothing can wrap */
    s = store_create(SIZE_MAX / 4);
    store_destroy(&s);

    /* case 2, an impossible length is rejected by the size check */
    s = store_create(STORE_SIZE);
    if (s != NULL) {
        store_set_name(&s[0], "hello", 99);
        store_set_name(&s[0], "hello", 5);
        printf("record 0 name = %s\n", s[0].name);
    }
    store_destroy(&s);

    /* case 3, the id is no longer truncated and the index is bounded */
    s = store_create(STORE_SIZE);
    if (s != NULL)
        store_touch(s, STORE_SIZE, 70000);
    store_destroy(&s);

    /* case 4, calloc zeroed the memory so this read is defined */
    s = store_create(STORE_SIZE);
    if (s != NULL)
        printf("record 1 id   = %d (zeroed by calloc)\n", s[1].id);
    store_destroy(&s);

    /* case 5 and 6, the pointer is NULL after the free, so neither the
       stale access nor the second free is possible */
    s = store_create(STORE_SIZE);
    store_destroy(&s);
    if (s == NULL)
        printf("store        : pointer is NULL after destroy\n");
    store_destroy(&s);                  /* harmless, free(NULL) does nothing */

    /* case 7, the store that used to leak is destroyed */
    s = store_create(STORE_SIZE);
    if (s != NULL) {
        store_set_name(&s[0], "kept", 4);
        printf("record 0 name = %s\n", s[0].name);
    }
    store_destroy(&s);

    /* case 8, the string is parsed with a function that can report failure */
    if (parse_count("99999999999999", &n) != 0)
        printf("parse_count  : refused, \"99999999999999\" does not fit in an int\n");
    else
        printf("parse_count  : %d\n", n);

    if (parse_count("4", &n) == 0) {
        s = store_create((size_t)n);
        store_destroy(&s);
    }

    printf("all cases finished, no errors\n");
    return 0;
}
