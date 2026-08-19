#include <stdio.h>
#include <stdint.h>

int main(void)
{
    //Since unsigned integer with 16bits becomes 8 bits after truncation
    // result should come as 50000 %2^8 or 256 = 80
    uint16_t large_value = 50000;
    uint8_t truncated_value = large_value;

    printf("Original value: %u\n", large_value);
    printf("Truncated value: %u\n", truncated_value);

    return 0;
}
