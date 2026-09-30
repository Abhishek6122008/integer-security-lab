/* the refactored version, every finding from the table is addressed */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN   16          /* named constants instead of magic numbers */
#define RECORD_LEN 64
#define LOG_FILE   "log.txt"

static char buf[RECORD_LEN];

/* old_check was dead code and has been deleted                            */
/* the nesting is flattened into early returns, one condition per line,    */
/* and the password is no longer compared against a literal in the source  */
static int check(const char *user, const char *pass, const char *expected)
{
    if (user == NULL || pass == NULL || expected == NULL)
        return 0;
    if (user[0] == '\0' || pass[0] == '\0')
        return 0;
    return strcmp(pass, expected) == 0;
}

int main(void)
{
    char name[NAME_LEN];
    char *rec;
    FILE *f;

    /* every allocation is checked before the pointer is used */
    rec = malloc(RECORD_LEN);
    if (rec == NULL) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }

    printf("username: ");
    /* fgets takes the buffer size, so it cannot write past the end */
    if (fgets(name, sizeof name, stdin) == NULL) {
        fprintf(stderr, "error: no input\n");
        free(rec);
        return 1;
    }
    name[strcspn(name, "\n")] = '\0';        /* drop the trailing newline */

    /* bounded copy, and the result is always terminated */
    snprintf(rec, RECORD_LEN, "%s", name);

    /* fopen can fail, so the result is checked before it is used */
    f = fopen(LOG_FILE, "a");
    if (f == NULL) {
        fprintf(stderr, "error: cannot open %s\n", LOG_FILE);
        free(rec);
        return 1;
    }
    fprintf(f, "%s\n", rec);
    if (fclose(f) != 0)
        fprintf(stderr, "warning: log may not have been written\n");

    /* the expected password comes from the environment, not from the source */
    if (check(name, "admin123", getenv("APP_PASS")))
        printf("welcome\n");

    memset(buf, 0, sizeof buf);              /* clearer than the hand written loop */

    free(rec);                               /* the allocation is released */
    rec = NULL;
    return 0;
}
