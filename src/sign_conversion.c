#include <stdio.h>
#include <stdint.h>

int main(void)
{
    //here we are changing from signed to unsigned 
    //the problem here is unsigned cannot go to negative its limits are double that of signed but in the possitive ingigeres only
    //this is because signed has one bit assigned to sign and 31 to integers
    int32_t signed_value = -10;
    uint32_t unsigned_value = signed_value;

    printf("Signed value: %d\n", signed_value);
    printf("After conversion to uint32_t: %u\n", unsigned_value);

    return 0;
}
