#include <stdio.h>

#include "getter.h"

int main() {
    int rc;
    int32_t val;

    rc = feature_switch_initialise();

    printf("RC: %d\n", rc);

    val = 1;

    while (0 == rc) {
        printf("Checking\n");
        rc = feature_switch_read(64, sizeof(int32_t), &val);
        if (val > 1) {
            break;
        }
        printf("Sleeping\n\n");
        sleep(1);
    }

    printf("RC: %d\n", rc);
    printf("Val: %d\n", val);
}
