#include <stdio.h>
#include <limits.h>

int main(void)
{
    int large_integer = 16777217;

    if ((long long)large_integer > 16777216LL)
    {
        printf("Warning: value may lose precision when converted to float.\n");
    }
    else
    {
        float converted_value = (float)large_integer;
        printf("Safe float conversion: %.0f\n", converted_value);
    }

    return 0;
}
