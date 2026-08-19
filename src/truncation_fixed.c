#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void)
{
    //for this file we will just check if the unsigned integer is less than or equal to 2^8 if not we cannot convert safely
    uint16_t large_value = 50000;

    if (large_value <= UINT8_MAX)
    {
        uint8_t converted_value = (uint8_t)large_value;
        printf("Safe conversion: %u\n", converted_value);
    }
    else
    {
        printf("Error: %u cannot safely fit in uint8_t.\n", large_value);
    }

    return 0;
}
