#include "drivers/video/FrameBuffer.h"


void _start(framebuffer_t* lfb) {
    #ifndef OMEGA_BIOS_BOOTLOADER
    while (1) {
        __asm__ __volatile__("hlt");
    }
    #endif
}
