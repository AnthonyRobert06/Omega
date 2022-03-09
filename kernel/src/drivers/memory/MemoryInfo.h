#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <stdint.h>

typedef struct {
    uint32_t type;
    void* segPhys;
    void* segVirt;
    uint64_t nPages;
    uint64_t attr;
} memdesc_t;


typedef struct {
    memdesc_t* mMap;
    uint64_t mMapSize;
    uint64_t mMapDescriptorSize;
} meminfo_t;


#endif
