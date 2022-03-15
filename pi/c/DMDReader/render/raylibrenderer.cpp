#include <iostream>
#include <assert.h> 
#include <chrono>
#include <thread>

#include <raylib.h>

#include "framerenderer.h"
#include "raylibrenderer.h"


RaylibRenderer::RaylibRenderer(int width1, int height1, int px_radius1, int px_spacing1, int bits_per_pixel) {
	width = width1;
	height = height1;
	px_radius = px_radius1;
    px_spacing = px_spacing1;

    palette_size = 1 << bits_per_pixel;
    palette = new Color[palette_size];
    if (palette) {
        for (int i = 0; i < palette_size; i++) {
            palette[i]=Fade(RED, i*((float)1/(palette_size-1)));
        }
    }

	InitWindow(width, height, "DMD display");
}

RaylibRenderer::~RaylibRenderer() {
	CloseWindow();
    if (palette) {
        delete[] palette;
    }
}


int RaylibRenderer::showImage(DMDFrame* f) {

    if (!(palette)) {
        return -1;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    int c_y = px_radius + px_spacing;
    int c_x = 0;

    int max_r = f->get_height();
    int max_c = f->get_width();
    int bpp = f->get_bitsperpixel();
    uint8_t pixel_mask = f->get_pixelmask();

    int pixel_bit = 8;

    int pixel_index = 0;

    uint8_t* databuff = f->get_data();

    for (int r = 0; r < max_r; r++) {
        c_x = px_radius + px_spacing;

        for (int c = 0; c < max_c; c++) {

            pixel_bit -= bpp;
            if (pixel_bit < 0) {
                pixel_bit += 8;
                pixel_index += 1;
            }
            uint8_t d = databuff[pixel_index];
            uint8_t pv = ((d >> pixel_bit) & pixel_mask);

            assert(pv < palette_size);
            DrawCircle(c_x, c_y, px_radius, palette[pv]);
            c_x += 2 * px_radius + px_spacing;
        }

        c_y += 2 * px_radius + px_spacing;
    }

    EndDrawing();

    return 0;
};