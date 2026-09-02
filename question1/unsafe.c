#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    // volatile stops gcc from folding these at compile time, so the
    // conversions really happen while the program runs
    volatile int big = 70000;
    short s = big;
    printf("int %d -> short %d\n", big, s);

    volatile int neg = -1;
    unsigned int u = neg;
    printf("signed %d -> unsigned %u\n", neg, u);

    volatile float f = 3.9e9f;
    int i = f;
    printf("float %.1f -> int %d\n", f, i);

    const char *text = "99999999999999";
    int n = atoi(text);
    printf("string \"%s\" -> int %d\n", text, n);

    // the truncated value from above is used as an array index,
    // 4464 % 100 = 64, still far outside a 10 byte buffer
    char *buf = malloc(10);
    buf[s % 100] = 'x';
    printf("wrote to buf[%d]\n", s % 100);
    free(buf);

    return 0;
}
