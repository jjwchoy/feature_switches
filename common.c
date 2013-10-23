#include "common.h"

typedef struct feature_switch_page_layout_t {
    uint8_t magic_number_p[8];
    uint8_t page_number_p[4];
    uint8_t page_count_p[4];
    uint8_t unused_p[16];
    uint8_t block_info_p[32];
} feature_switch_page_layout_t;

static const uint8_t MAGIC[8] = {
    'F', 'S', '2', '0', '1', '3', '1', '0'
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
        void *data,
        feature_switch_pages_t *pages_info) {
    feature_switch_page_layout_t *page_layout;

    page_layout = data;

    if (0 != memcmp(MAGIC, page_layout->magic_number_p, sizeof(MAGIC))) {
        return FEATURE_SWITCH_ERR_INVALID;
    }

    pages_info->page_count = feature_switch_read_uint32(page_layout->page_number);
    pages_info->pages = data;

    return FEATURE_SWITCH_OK;
}

feature_switch_err_t
feature_switch_block_info(
        feature_switch_pages_t *pages_info,
        uint32_t block_number,
        feature_switch_block_info_t *block_info) {
    feature_switch_err_t rc;
    uint32_t required_page;
    uint32_t block_within_page;
    feature_switch_page_t *page;
    int i;

    required_page = block_number / FEATURE_SWITCH_BLOCKS_PER_PAGE;
    block_within_page = block_number % FEATURE_SWITCH_BLOCKS_PER_PAGE;

    if (0 == block_within_page) {
        // The first block on every page contains metadata
        // Therefore this is an invalid block number
        return FEATURE_SWITCH_ERR_INVALID;
    }

    page = pages_info->pages;

    for (i = 0; i < pages_info->page_count; ++i) {
        int32_t page_number;
        feature_switch_page_t *page;
        feature_switch_page_layout_t *page_layout;

        page_layout = (feature_switch_page_layout_t *)page;
        page_number = feature_switch_read_uint32(page_layout->page_number_p);

        if (required_page < page_number) {
            // Requested a defunct page
            *type = FEATURE_SWITCH_TYPE_DEFUNCT;
            return FEATURE_SWITCH_OK;
        }

        if (page_number == required_page) {
            uint8_t type = page_layout->block_info_p[block_within_page / 2];

            if (block_within_page % 2) {
                type <<= 4;
            }

            type &= 0xf;

            block_info->type = (feature_switch_type_t)type;
            block_info->data = page->blocks + block_within_page;
            return FEATURE_SWITCH_OK;
        }
    }
    block_info->type = FEATURE_SWITCH_TYPE_EMPTY;
    block_info->data = NULL;
    return FEATURE_SWITCH_OK;
}

feature_switch_err_t
feature_switch_init_page(void *data) {
    feature_switch_page_info_t page_info[1];
}
        
