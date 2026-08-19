#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void)
{
    int32_t a = INT32_MAX;
    int32_t b = 1;

    if (b > 0 && a > INT32_MAX - b)
    {
        printf("Error: addition would cause integer overflow.\n");
    }
    else
    {
        int32_t result = a + b;
        printf("Safe result: %d\n", result);
    }

    return 0;
}
