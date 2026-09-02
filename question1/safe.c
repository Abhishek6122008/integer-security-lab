#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(void)
{
    int big = 70000;
    if (big < SHRT_MIN || big > SHRT_MAX)
        printf("Error: %d does not fit in a short\n", big);
    else
        printf("int %d -> short %d\n", big, (short)big);

    int neg = -1;
    if (neg < 0)
        printf("Error: %d is negative, cannot make it unsigned\n", neg);
    else
        printf("signed %d -> unsigned %u\n", neg, (unsigned int)neg);

    float f = 3.9e9f;
    if (f < (float)INT_MIN || f > (float)INT_MAX)
        printf("Error: float %.1f is outside int range\n", f);
    else
        printf("float %.1f -> int %d\n", f, (int)f);

    const char *text = "99999999999999";
    long v = strtol(text, NULL, 10);
    if (v < INT_MIN || v > INT_MAX)
        printf("Error: string \"%s\" is too big for an int\n", text);
    else
        printf("string \"%s\" -> int %d\n", text, (int)v);

    return 0;
}
