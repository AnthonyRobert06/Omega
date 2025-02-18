#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stddef.h>
#include "../../util/string.h"

#define DEFAULT_CANVAS_GAPSIZE 300


typedef struct {
    void* baseAddr;
    size_t bufferSize;
    unsigned int width;
    unsigned int height;
    unsigned int ppsl;      // Pixels per scanline.
} framebuffer_t;


typedef struct {
    unsigned char magic[2];
    unsigned char mode;
    unsigned char chsize;
} psf1_header_t;


typedef struct {
    psf1_header_t* header;
    void* glyphBuffer;
} psf1_font_t;


typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int prevX;
    unsigned int gapSize;
    framebuffer_t* lfb;
    psf1_font_t* font;
} canvas_t;


void putChar(framebuffer_t* framebuffer, psf1_font_t* psf1_font, unsigned int color, char chr, unsigned int xOff, unsigned int yOff);
void kwrite(canvas_t* canvas, const char* const STR, unsigned int color);
void clearScreen(canvas_t* canvas, unsigned int color);

#endif
