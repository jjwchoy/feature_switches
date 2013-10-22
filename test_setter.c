#include <stdio.h>

#include "setter.h"

int main(int argc) {
    int rc;
    int32_t val;

    rc = feature_switch_initialise();

    printf("RC: %d\n", rc);

    rc = feature_switch_read(64, sizeof(int32_t), &val);
    printf("RC: %d\n", rc);
    printf("Val: %d\n", val);

    val = argc;
    rc = feature_switch_set(64, sizeof(int32_t), &val);
    printf("RC: %d\n", rc);
    printf("Val: %d\n", val);

    rc = feature_switch_read(64, sizeof(int32_t), &val);
    printf("RC: %d\n", rc);
    printf("Val: %d\n", val);

    return 0;
}
