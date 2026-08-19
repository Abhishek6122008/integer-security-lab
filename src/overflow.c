#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void)
{
    //here just for demonstration we overflow 32 bit integer by adding 1 to its max value 
    int32_t a = INT32_MAX;
    int32_t b = 1;
    int32_t result = a + b;

    printf("Maximum int32_t value: %d\n", a);
    printf("Result after adding 1: %d\n", result);

    return 0;
}
