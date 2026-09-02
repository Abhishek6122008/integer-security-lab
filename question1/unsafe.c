#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int big = 70000;
    short s = big;
    printf("int %d -> short %d\n", big, s);

    int neg = -1;
    unsigned int u = neg;
    printf("signed %d -> unsigned %u\n", neg, u);

    float f = 3.9e9f;
    int i = f;
    printf("float %.1f -> int %d\n", f, i);

    const char *text = "99999999999999";
    int n = atoi(text);
    printf("string \"%s\" -> int %d\n", text, n);

    return 0;
}
