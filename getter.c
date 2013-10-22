#include "getter.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static const unsigned char *SWITCHES = NULL;
static const size_t SWITCHES_MAX_SIZE = 1024 * 1024;
//static const char *SWITCHES_FILE = "/var/data/feature_switch/shared";
static const char *SWITCHES_FILE = "shmem.test";

static sem_t *SWITCH_MUTEX;

int
feature_switch_initialise(void) {
    int fd = open(SWITCHES_FILE, O_RDONLY);
    SWITCHES = mmap(NULL, SWITCHES_MAX_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    
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

int
feature_switch_read(
        size_t switch_offset,
        size_t switch_len,
        void *buf) {
    uint32_t file_size;
    uint32_t *file_size_p;

    if (!SWITCHES) {
        return FEATURE_SWITCH_UNINITIALISED;
    }

    sem_wait(SWITCH_MUTEX);

    // First 4 bytes of the SWITCHES file are the total file size
    file_size_p = (uint32_t*)SWITCHES;
    // Read the file size. It is stored in network byte order
    file_size = ntohl(*file_size_p);

    if ((switch_offset + switch_len) > file_size) {
        sem_post(SWITCH_MUTEX);
        return FEATURE_SWITCH_INVALID;
    }

    memcpy(buf, SWITCHES + switch_offset, switch_len);

    sem_post(SWITCH_MUTEX);
    return 0;
}

int
feature_switch_read_uint8(
        size_t switch_offset,
        uint8_t *out) {
    return feature_switch_read(switch_offset, sizeof(uint8_t), out);
}

int
feature_switch_read_uint16(
        size_t switch_offset,
        uint16_t *out) {
    uint16_t val;
    int rc;
    rc = feature_switch_read(switch_offset, sizeof(uint16_t), &val);
    *out = ntohs(val);
    return rc;
}

int
feature_switch_read_uint32(
        size_t switch_offset,
        uint32_t *out) {
    uint32_t val;
    int rc;
    rc = feature_switch_read(switch_offset, sizeof(uint32_t), &val);
    *out = ntohl(val);
    return rc;
}
