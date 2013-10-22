#include "setter.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

static unsigned char *SWITCHES = NULL;
static const int32_t SWITCHES_MAX_SIZE = 1024 * 1024;
//static const char *SWITCHES_FILE = "/var/data/feature_switch/shared";
static const char *SWITCHES_FILE = "shmem.test";

static sem_t *SWITCH_MUTEX;

static feature_switch_err_t
feature_switch_read_nolock(
        size_t switch_offset,
        size_t switch_len,
        void *buf);

static feature_switch_err_t
feature_switch_set_nolock(
        size_t switch_offset,
        size_t switch_len,
        void *value);

feature_switch_err_t
feature_switch_initialise(void) {
    int fd = open(SWITCHES_FILE, O_RDWR);

    // Initialise the file
    int32_t file_size_n = htonl(1024);

    write(fd, &file_size_n, sizeof(int32_t));
    lseek(fd, 0, SEEK_SET);
    fsync(fd);

    SWITCHES = mmap(NULL, SWITCHES_MAX_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (MAP_FAILED == SWITCHES) {
        perror("Couldn't open switches file");
        return errno;
    }

    SWITCH_MUTEX = sem_open("/featureswitches", O_CREAT, 0777, 1);

    if (SEM_FAILED == SWITCH_MUTEX) {
        perror("Couldn't create or open semaphore\n");
        return errno;
    }

    return 0;
}

feature_switch_err_t
feature_switch_read(
        size_t switch_offset,
        size_t switch_len,
        void *buf) {
    feature_switch_err_t rc;

    if (!SWITCHES) {
        return FEATURE_SWITCH_ERR_UNINITIALISED;
    }

    sem_wait(SWITCH_MUTEX);
    rc = feature_switch_read_nolock(switch_offset, switch_len, buf);
    sem_post(SWITCH_MUTEX);
    return rc;
}

feature_switch_err_t
feature_switch_read_nolock(
        size_t switch_offset,
        size_t switch_len,
        void *buf) {
    uint32_t file_size;
    uint32_t *file_size_p;

    // First 4 bytes of the SWITCHES file are the total file size
    file_size_p = (uint32_t*)SWITCHES;
    // Read the file size. It is stored in network byte order
    file_size = ntohl(*file_size_p);

    if ((switch_offset + switch_len) > file_size) {
        sem_post(SWITCH_MUTEX);
        return FEATURE_SWITCH_ERR_INVALID;
    }

    memcpy(buf, SWITCHES + switch_offset, switch_len);
    return FEATURE_SWITCH_OK;
}

feature_switch_err_t
feature_switch_set(
        size_t switch_offset,
        size_t switch_len,
        void *value) {
    feature_switch_err_t rc;

    if (!SWITCHES) {
        return FEATURE_SWITCH_ERR_UNINITIALISED;
    }

    sem_wait(SWITCH_MUTEX);
    rc = feature_switch_set_nolock(switch_offset, switch_len, value);
    sem_post(SWITCH_MUTEX);

    return rc;
}


feature_switch_err_t
feature_switch_set_nolock(
        size_t switch_offset,
        size_t switch_len,
        void *value) {
    uint32_t file_size;
    uint32_t *file_size_p;

    // First 4 bytes of the SWITCHES file are the total file size
    file_size_p = (uint32_t*)SWITCHES;
    // Read the file size. It is stored in network byte order
    file_size = ntohl(*file_size_p);

    if ((switch_offset + switch_len) > file_size) {
        sem_post(SWITCH_MUTEX);
        return FEATURE_SWITCH_ERR_INVALID;
    }

    memcpy(SWITCHES + switch_offset, value, switch_len);

    sem_post(SWITCH_MUTEX);
    return 0;
}

feature_switch_err_t
feature_switch_set_bit(
        size_t switch_offset,
        unsigned char bit_offset,
        int value) {
    uint8_t val;
    feature_switch_err_t rc;

    sem_wait(SWITCH_MUTEX);

    rc = feature_switch_read_nolock(switch_offset, sizeof(uint8_t), &val);

    if (FEATURE_SWITCH_OK == rc) {
        if (value) {
            val |= (1 << bit_offset);
        } else {
            val &= ~(1 << bit_offset);
        }
        rc = feature_switch_set_nolock(switch_offset, sizeof(uint8_t), &val);
    }

    sem_post(SWITCH_MUTEX);
    return rc;
}

feature_switch_err_t
feature_switch_set_uint8(
        size_t switch_offset,
        uint8_t value) {
    return feature_switch_set(switch_offset, sizeof(uint8_t), &value);
}

feature_switch_err_t
feature_switch_set_uint16(
        size_t switch_offset,
        uint16_t value) {
    uint16_t val = htons(value);
    return feature_switch_set(switch_offset, sizeof(uint16_t), &val);
}

feature_switch_err_t
feature_switch_set_uint32(
        size_t switch_offset,
        uint32_t value) {
    uint32_t val = htonl(value);
    return feature_switch_set(switch_offset, sizeof(uint32_t), &val);
}
