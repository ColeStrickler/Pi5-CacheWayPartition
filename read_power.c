#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>



void main()
{
    printf("CLUSTERPWRSTAT 0x%x\n", read_CLUSTERPWRSTAT_EL1());
    return;
}