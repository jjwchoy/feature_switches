#ifndef FEATURE_SWITCH_GETTER_H
#define FEATURE_SWITCH_GETTER_H

#include <stddef.h>
#include <stdint.h>

typedef enum feature_switch_err_t {
    FEATURE_SWITCH_OK,
    FEATURE_SWITCH_UNINITIALISED,
    FEATURE_SWITCH_INVALID
} feature_switch_err_t;

int
feature_switch_initialise(void);

int
feature_switch_read(
        size_t switch_offset,
        size_t switch_len,
        void *buf);

int
feature_switch_read_uint8(
        size_t switch_offset,
        uint8_t *out);

int
feature_switch_read_uint16(
        size_t switch_offset,
        uint16_t *out);

int
feature_switch_read_uint32(
        size_t switch_offset,
        uint32_t *out);

#endif
