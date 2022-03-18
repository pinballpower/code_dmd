#pragma once
#include "../dmd/color.h"

#include "raylib.h"
#include "framerenderer.h"

class RaylibRenderer: FrameRenderer
{
public:

	RaylibRenderer();
	~RaylibRenderer();
	virtual void render_frame(DMDFrame* f);
	void set_palette(DMDPalette*);

	void set_display_parameters(int width, int height, int px_radius, int px_spacing, int bitsperpixel);
	void start_display();
	virtual bool configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_renderer);

private:

	int width = 0;
	int height = 0;
	int px_radius = 0;
	int px_spacing = 0;
	int palette_size = 0;

	Color *palette;
};