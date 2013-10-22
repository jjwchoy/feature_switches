#ifndef FEATURE_SWITCH_COMMON_H
#define FEATURE_SWITCH_COMMON_H

enum {
    FEATURE_SWITCH_PAGE_SIZE = 4096,
    FEATURE_SWITCH_BLOCK_SIZE = 64,
    FEATURE_SWITCH_BLOCKS_PER_PAGE = 64
};

typedef enum feature_switch_err_t {
    FEATURE_SWITCH_OK,
    FEATURE_SWITCH_ERR_UNINITIALISED,
    FEATURE_SWITCH_ERR_INVALID
} feature_switch_err_t;

typedef enum feature_switch_type_t {
    FEATURE_SWITCH_TYPE_EMPTY,
    FEATURE_SWITCH_TYPE_DEFUNCT,
    FEATURE_SWITCH_TYPE_BITMAP,
    FEATURE_SWITCH_TYPE_
} feature_switch_type_t;

enum {
    FEATURE_SWITCH_BITMASK_BLOCK_NUM_ELEMENTS = 168,
    FEATURE_SWITCH_BITMASK_BLOCK_DATA_OFFSET = 42,

    FEATURE_SWITCH_UINT8_BLOCK_NUM_ELEMENTS = 51,
    FEATURE_SWITCH_UINT8_BLOCK_DATA_OFFSET = 13,

    FEATURE_SWITCH_UINT16_BLOCK_NUM_ELEMENTS = 28,
    FEATURE_SWITCH_UINT16_BLOCK_DATA_OFFSET = 7,

    FEATURE_SWITCH_UINT32_BLOCK_NUM_ELEMENTS = 15,
    FEATURE_SWITCH_UINT32_BLOCK_DATA_OFFSET = 4,

    FEATURE_SWITCH_UINT64_BLOCK_NUM_ELEMENTS = 7,
    FEATURE_SWITCH_UINT64_BLOCK_DATA_OFFSET = 2
};

typedef struct feature_switch_block_info_t {
    feature_switch_type_t type;
    void *data;
} feature_switch_block_info_t;

uint8_t
feature_switch_read_uint8(const void *data);

uint16_t
feature_switch_read_uint16(const void *data);

uint32_t
feature_switch_read_uint32(const void *data);

uint64_t
feature_switch_read_uint64(const void *data);

void
feature_switch_write_uint8(void *buf, uint8_t val);

void
feature_switch_write_uint16(void *buf, uint16_t val);

void
feature_switch_write_uint32(void *buf, uint32_t val);

void
feature_switch_write_uint64(void *buf, uint64_t val);

feature_switch_err_t
feature_switch_parse_page_meta(
        const void *data,
        uint32_t *page_number,
        uint32_t *page_count,
        const void **block_info);

feature_switch_type_t
feature_switch_block_info(
        void *data,
        uint32_t block_number,
        feature_switch_block_info_t *block_info);

#endif
