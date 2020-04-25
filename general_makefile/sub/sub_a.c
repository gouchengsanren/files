#include <stdio.h>

void sub_a()
{
    printf("%s\n", __func__);
#ifdef SUBA_VAL
    printf("%s %d\n", __func__, SUBA_VAL);
#endif
}
