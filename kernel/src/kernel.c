#include "drivers/video/FrameBuffer.h"
#include "drivers/memory/MemoryMap.h"
#include "drivers/memory/MemoryInfo.h"
#include "drivers/video/FrameBuffer.h"
#include "util/string.h"
#include <stdint.h>

#define OMEGA_MMAP_DISPLAY

canvas_t canvas = {
    .x = 10,
    .y = 30,
    .prevX = 10,
    .gapSize = DEFAULT_CANVAS_GAPSIZE
};

void _start(framebuffer_t* lfb, psf1_font_t* font, meminfo_t meminfo) {
    canvas.lfb = lfb;
    canvas.font = font;

    #ifdef OMEGA_MMAP_DISPLAY
    uint64_t mMapEntries = getMMapEntries(meminfo);
    canvas.gapSize = 400;       // Increases spacing on max y by 400.

    for (int i = 0; i < mMapEntries; ++i) {  
        memdesc_t* desc = mMapGetIterationHelper(meminfo, i);       // Get the descriptor for each entry.
        uint64_t segmentSizeKB = (desc->nPages * 4096);             // Get segment size.

        if (segmentSizeKB > 0) {        // Don't divide by zero! That would be hell to debug.
            segmentSizeKB /= 1024;
        }

        // Display segment information.
        kwrite(&canvas, hex2str((uint64_t)desc->segPhys), 0x800080);
        kwrite(&canvas, " => ", 0xFFFFFFFF);
        kwrite(&canvas, MSEGMENTS[desc->type], 0xFF0000FF);
        kwrite(&canvas, " => ", 0xFFFFFF);
        kwrite(&canvas, dec2str(segmentSizeKB), 0xFFC0CB);
        kwrite(&canvas, " KB", 0xFFC0CB);
        kwrite(&canvas, "\n", 0xFFFFFFFF);
    }

    // Restore gap size.
    canvas.gapSize = DEFAULT_CANVAS_GAPSIZE;
    #endif

    while (1) {
        __asm__ __volatile__("hlt");
    }
}
