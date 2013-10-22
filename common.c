#include "common.h"

static const uint8_t MAGIC[8] = {
    'F', 'S', 9, 2, 4, 3, 'J', 'C'
};

uint8_t
feature_switch_read_uint8(const void *data) {
    return *(uint8_t *)data;
}

uint16_t
feature_switch_read_uint16(const void *data) {
    uint16_t val = *(uint16_t *)data;
    return be16toh(val);
}

uint32_t
feature_switch_read_uint32(const void *data) {
    uint32_t val = *(uint32_t *)data;
    return be32toh(val);
}

uint64_t
feature_switch_read_uint64(const void *data) {
    uint64_t val = *(uint64_t *)data;
    return be64toh(val);
}

void
feature_switch_write_uint8(void *buf, uint8_t val) {
    memcpy(buf, &val, sizeof(uint8_t));
}

void
feature_switch_write_uint16(void *buf, uint16_t val) {
    val = htobe16(val);
    memcpy(buf, &val, sizeof(uint16_t));
}

void
feature_switch_write_uint32(void *buf, uint32_t val) {
    val = htobe32(val);
    memcpy(buf, &val, sizeof(uint32_t));
}

void
feature_switch_write_uint64(void *buf, uint64_t val) {
    val = htobe64(val);
    memcpy(buf, &val, sizeof(uint32_t));
}

/* Page meta structure
 *
 * 8 bytes - MAGIC NUMBER
 * 4 bytes - page number unsigned
 * 4 bytes - page count unsigned
 * 16 bytes - unused
 * 32 bytes - block types
 */
feature_switch_err_t
feature_switch_parse_page_meta(
        const void *data,
        uint32_t *page_number,
        uint32_t *page_count,
        const void **block_info) {
    const uint8_t *p;

    static const int page_number_offset = 8;
    static const int page_count_offset = 12;
    static const int block_info_offset = 32;

    p = data;

    if (0 != memcmp(MAGIC, p, sizeof(MAGIC))) {
        return FEATURE_SWITCH_ERR_INVALID;
    }

    *page_number = feature_switch_read_uint32(p + page_number_offset);
    *page_count = feature_switch_read_uint32(p + page_count_offset);
    *block_info = p + block_info_offset;

    return FEATURE_SWITCH_OK;
}

feature_switch_err_t
feature_switch_block_info(
        void *data,
        uint32_t block_number,
        feature_switch_block_info_t *block_info) {
    feature_switch_err_t rc;
    uint8_t *page;
    uint32_t page_number;
    uint32_t page_count;
    const uint8_t *page_blocks;
    uint32_t required_page;
    uint32_t block_within_page;

    page = data;

    required_page = block_number / FEATURE_SWITCH_BLOCKS_PER_PAGE;
    block_within_page = block_number % FEATURE_SWITCH_BLOCKS_PER_PAGE;

    if (0 == block_within_page) {
        // The first block on every page contains metadata
        // Therefore this is an invalid block number
        return FEATURE_SWITCH_ERR_INVALID;
    }

    rc = feature_switch_parse_page_meta(
            page,
            &page_number,
            &page_count,
            &page_blocks);

    if (FEATURE_SWITCH_OK != rc) {
        return rc;
    }

    while (page_number < required_page) {
        if (required_page < page_number) {
            // Requested a defunct page
            *type = FEATURE_SWITCH_TYPE_DEFUNCT;
            return FEATURE_SWITCH_OK;
        }

        if (page_number == required_page) {
            // On the right page
            // Lookup the appropriate block
            size_t block_offset = FEATURE_SWITCH_BLOCK_SIZE * block_within_page;
            uint8_t *type_ptr = page_blocks + (block_within_page / 2);
            uint8_t type = *type_ptr;

            if (block_within_page % 2) {
                type <<= 4;
            }

            type &= 0xf;

            block_info->type = (feature_switch_type_t)type;
            block_info->data = page + block_offset;
            return FEATURE_SWITCH_OK;
        }

        page += FEATURE_SWITCH_PAGE_SIZE;
    }
    block_info->type = FEATURE_SWITCH_TYPE_EMPTY;
    block_info->data = NULL;
    return FEATURE_SWITCH_OK;
}
        
