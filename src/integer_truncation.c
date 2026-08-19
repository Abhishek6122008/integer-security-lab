#include <stdio.h>
#include <stdint.h>

int main(void)
{
    uint16_t large_value = 50000;
    uint8_t truncated_value = large_value;

    printf("Original value: %u\n", large_value);
    printf("Truncated value: %u\n", truncated_value);

    return 0;
}
