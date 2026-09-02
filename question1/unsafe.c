#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    // int is 4 bytes and short is only 2 bytes, so 70000 does not fit
    // the extra bits are thrown away and we get 4464 instead
    int big = 70000;
    short s = big;
    printf("int %d -> short %d\n", big, s);

    // unsigned cannot hold negative numbers, so -1 is reinterpreted
    // as the bit pattern all 1s, which prints as 4294967295
    int neg = -1;
    unsigned int u = neg;
    printf("signed %d -> unsigned %u\n", neg, u);

    // 3.9e9 is bigger than INT_MAX (about 2.1e9), converting it to int
    // is undefined behaviour so the value we get is garbage
    float f = 3.9e9f;
    int i = f;
    printf("float %.1f -> int %d\n", f, i);

    // atoi cannot report an error, the string is too big for an int
    // so it just returns a wrong value silently
    const char *text = "99999999999999";
    int n = atoi(text);
    printf("string \"%s\" -> int %d\n", text, n);

    return 0;
}
