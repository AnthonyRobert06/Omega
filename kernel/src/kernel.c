#include "drivers/video/FrameBuffer.h"
#include "drivers/memory/MemoryMap.h"
#include "drivers/memory/MemoryInfo.h"
#include "drivers/video/FrameBuffer.h"
#include "util/string.h"
#include <stdint.h>


canvas_t canvas = {
    .x = 10,
    .y = 10,
    .prevX = 10
};


void _start(framebuffer_t* lfb, psf1_font_t* font, meminfo_t meminfo) {
    canvas.lfb = lfb;
    canvas.font = font;

    kwrite(&canvas, "\n", 0xFFFFFFFF);

    #ifdef OMEGA_MMAP_DISPLAY
    uint64_t mMapEntries = getMMapEntries(meminfo);

    for (int i = 0; i < mMapEntries; ++i) {
        memdesc_t* desc = mMapGetIterationHelper(meminfo, i);
        kwrite(&canvas, hex2str((uint64_t)desc->segPhys), 0x800080);
        kwrite(&canvas, " => ", 0xFFFFFFFF);
        kwrite(&canvas, MSEGMENTS[desc->type], 0xFF0000FF);
        kwrite(&canvas, "\n", 0xFFFFFFFF);
    }
    #endif

    while (1) {
        __asm__ __volatile__("hlt");
    }
}
