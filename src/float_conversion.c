#include <stdio.h>

int main(void)
{
    int large_integer = 16777217;
    float converted_value = large_integer;

    printf("Original int value: %d\n", large_integer);
    printf("Converted float value: %.0f\n", converted_value);

    return 0;
}
