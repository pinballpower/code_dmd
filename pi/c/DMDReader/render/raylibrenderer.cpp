#include <iostream>
#include <assert.h> 
#include <chrono>
#include <thread>
#include <assert.h>

#include <raylib.h>

#include "framerenderer.h"
#include "raylibrenderer.h"

RaylibRenderer::RaylibRenderer() {
    width = height = px_radius = px_spacing = 0;
}

void RaylibRenderer::set_display_parameters(int width, int height, int px_radius, int px_spacing, int bitsperpixel) {

    assert((bitsperpixel > 0) && (bitsperpixel <= 8));

	this->width = width;
	this->height = height;
	this->px_radius = px_radius;
    this->px_spacing = px_spacing;

    palette_size = 1 << bitsperpixel;
    palette = new Color[palette_size];
    if (palette) {
        for (int i = 0; i < palette_size; i++) {
            palette[i]=Fade(RED, i*((float)1/(palette_size-1)));
        }
    }
}

RaylibRenderer::~RaylibRenderer() {
	CloseWindow();
    if (palette) {
        delete[] palette;
    }
}


void RaylibRenderer::render_frame(DMDFrame* f) {

    if (!(palette)) {
        BOOST_LOG_TRIVIAL(error) << "can't render frame, no palette found";
        return;
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
};

void RaylibRenderer::start_display() {
    InitWindow(width, height, "DMD display");
}


bool RaylibRenderer::configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_renderer) {

    int bitsperpixel = pt_general.get("bitsperpixel", 0);
    if (!bitsperpixel) {
        BOOST_LOG_TRIVIAL(error) << "couldn't detect bits/pixel";
        return false;
    }

    int rows = pt_general.get("rows", 0);
    int columns = pt_general.get("columns", 0);
    if ((rows <= 0) || (columns <= 0)) {
        BOOST_LOG_TRIVIAL(error) << "rows or columns not detected correctly";
        return false;
    }

    int width = pt_renderer.get("width", 1280);
    int height = pt_renderer.get("height", 320);
    int px_spacing = pt_renderer.get("spacing", 2);
    int radius = ((width / columns) - px_spacing) / 2;

    set_display_parameters(width, height, radius, px_spacing, bitsperpixel);

    start_display();

    return true;
}