#ifndef FEATURE_SWITCH_GETTER_H
#define FEATURE_SWITCH_GETTER_H

#include <stddef.h>
#include <stdint.h>

#include "common.h"

feature_switch_err_t
feature_switch_initialise(void);

feature_switch_err_t
feature_switch_read(
        size_t switch_offset,
        size_t switch_len,
        void *buf);

feature_switch_err_t
feature_switch_read_bit(
        size_t switch_offset,
        unsigned char bit_offset,
        int *out);

feature_switch_err_t
feature_switch_read_uint8(
        size_t switch_offset,
        uint8_t *out);

feature_switch_err_t
feature_switch_read_uint16(
        size_t switch_offset,
        uint16_t *out);

feature_switch_err_t
feature_switch_read_uint32(
        size_t switch_offset,
        uint32_t *out);

#endif
