#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main(void)
{
    int32_t signed_value = -10;

    if (signed_value < 0)
    {
        printf("Error: negative value cannot be safely converted to uint32_t.\n");
    }
    else
    {
        uint32_t unsigned_value = (uint32_t)signed_value;
        printf("Safe conversion: %" PRIu32 "\n", unsigned_value);
    }

    return 0;
}
