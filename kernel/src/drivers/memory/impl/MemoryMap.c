#include "../MemoryMap.h"

const char* const MSEGMENTS[] = {
    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiACPIReclaimMemory",
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode"
};



uint64_t getMMapEntries(meminfo_t meminfo) {
    return meminfo.mMapSize / meminfo.mMapDescriptorSize;
}


memdesc_t* mMapGetIterationHelper(meminfo_t meminfo, int i) {
    return (memdesc_t*)((uint64_t)meminfo.mMap + (i * meminfo.mMapDescriptorSize));
}
