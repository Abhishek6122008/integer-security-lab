#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <math.h>

static int int_to_short(int v, short *out)
{
    if (v < SHRT_MIN || v > SHRT_MAX)
        return 0;
    *out = (short)v;
    return 1;
}

static int int_to_unsigned(int v, unsigned int *out)
{
    if (v < 0)
        return 0;
    *out = (unsigned int)v;
    return 1;
}

static int float_to_int(float f, int *out)
{
    if (isnan(f) || f < (float)INT_MIN || f > (float)INT_MAX)
        return 0;
    *out = (int)f;
    return 1;
}

static int string_to_int(const char *s, int *out)
{
    char *end;
    errno = 0;
    long v = strtol(s, &end, 10);
    if (end == s || *end != '\0')
        return 0;
    if (errno == ERANGE || v < INT_MIN || v > INT_MAX)
        return 0;
    *out = (int)v;
    return 1;
}

int main(void)
{
    int big = 70000;
    short s;
    if (int_to_short(big, &s))
        printf("int %d -> short %d\n", big, s);
    else
        printf("Error: %d does not fit in a short\n", big);

    int neg = -1;
    unsigned int u;
    if (int_to_unsigned(neg, &u))
        printf("signed %d -> unsigned %u\n", neg, u);
    else
        printf("Error: %d is negative, cannot convert to unsigned\n", neg);

    float f = 3.9e9f;
    int i;
    if (float_to_int(f, &i))
        printf("float %.1f -> int %d\n", f, i);
    else
        printf("Error: float %.1f is outside int range\n", f);

    const char *text = "99999999999999";
    int n;
    if (string_to_int(text, &n))
        printf("string \"%s\" -> int %d\n", text, n);
    else
        printf("Error: string \"%s\" is not a valid int\n", text);

    return 0;
}
