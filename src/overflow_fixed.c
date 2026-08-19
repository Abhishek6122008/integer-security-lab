#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void)
{
    //here we just check if the integer is not overflowing by the formula a>intmax - b 
    // we are also checking for b>0 since if it isnt there is no need to check for the other condition
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
