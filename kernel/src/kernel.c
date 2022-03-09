#include "drivers/video/FrameBuffer.h"


void _start(framebuffer_t* lfb) {
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
