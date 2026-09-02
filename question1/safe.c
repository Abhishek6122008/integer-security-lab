#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(void)
{
    // before putting an int into a short we check it against the short
    // limits from limits.h, if it does not fit we refuse the conversion
    int big = 70000;
    if (big < SHRT_MIN || big > SHRT_MAX)
        printf("Error: %d does not fit in a short\n", big);
    else
        printf("int %d -> short %d\n", big, (short)big);

    // unsigned can only hold zero or positive values, so any negative
    // number is rejected instead of becoming a huge positive number
    int neg = -1;
    if (neg < 0)
        printf("Error: %d is negative, cannot make it unsigned\n", neg);
    else
        printf("signed %d -> unsigned %u\n", neg, (unsigned int)neg);

    // we compare the float with the int limits first, so we never do the
    // undefined conversion that the unsafe version does
    float f = 3.9e9f;
    if (f < (float)INT_MIN || f > (float)INT_MAX)
        printf("Error: float %.1f is outside int range\n", f);
    else
        printf("float %.1f -> int %d\n", f, (int)f);

    // strtol returns a long which is bigger than int, so we can check the
    // result first and only then cast it down, which atoi cannot do
    const char *text = "99999999999999";
    long v = strtol(text, NULL, 10);
    if (v < INT_MIN || v > INT_MAX)
        printf("Error: string \"%s\" is too big for an int\n", text);
    else
        printf("string \"%s\" -> int %d\n", text, (int)v);

    return 0;
}
