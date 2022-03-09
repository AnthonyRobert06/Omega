#include "drivers/video/FrameBuffer.h"


void _start(framebuffer_t* lfb, psf1_font_t* font) {
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
