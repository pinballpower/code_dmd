#pragma once
#include "../dmd/color.h"

#include "raylib.h"
#include "framerenderer.h"

class RaylibRenderer: FrameRenderer
{
public:

	RaylibRenderer(int width1 = 128 * 11, int height1 = 32 * 11, int px_radius1 = 5, int px_spacing1 = 1, int bitsperpixel = 4);
	~RaylibRenderer();
	int showImage(DMDFrame* f);
	void set_palette(DMDPalette*);

private:

	int width = 0;
	int height = 0;
	int px_radius = 0;
	int px_spacing = 0;
	int palette_size = 0;

	Color *palette;
};