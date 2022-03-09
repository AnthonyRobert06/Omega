#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "MemoryInfo.h"
#include <stdint.h>

extern const char* const MSEGMENTS[];

uint64_t getMMapEntries(meminfo_t meminfo);
memdesc_t* mMapGetIterationHelper(meminfo_t meminfo, int i);

#endif
