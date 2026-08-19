#include <stdio.h>
#include <stdint.h>

int main(void)
{
    int32_t signed_value = -10;
    uint32_t unsigned_value = signed_value;

    printf("Signed value: %d\n", signed_value);
    printf("After conversion to uint32_t: %u\n", unsigned_value);

    return 0;
}
