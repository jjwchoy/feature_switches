#ifndef FEATURE_SWITCH_SETTER_H
#define FEATURE_SWITCH_SETTER_H

#include <stddef.h>
#include <stdint.h>

typedef enum feature_switch_err_t {
    FEATURE_SWITCH_OK,
    FEATURE_SWITCH_ERR_UNINITIALISED,
    FEATURE_SWITCH_ERR_INVALID
} feature_switch_err_t;

feature_switch_err_t
feature_switch_initialise(void);

feature_switch_err_t
feature_switch_read(
        size_t switch_offset,
        size_t switch_len,
        void *buf);

feature_switch_err_t
feature_switch_set(
        size_t switch_offset,
        size_t switch_len,
        void *value);

feature_switch_err_t
feature_switch_set_bit(
        size_t switch_offset,
        unsigned char bit_offset,
        int value);

feature_switch_err_t
feature_switch_set_uint8(
        size_t switch_offset,
        uint8_t value);

feature_switch_err_t
feature_switch_set_uint16(
        size_t switch_offset,
        uint16_t value);

feature_switch_err_t
feature_switch_set_uint32(
        size_t switch_offset,
        uint32_t value);

#endif
