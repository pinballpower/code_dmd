#include <cstdint>
#include <iostream>
#include <vector>
#include <assert.h>

#include "color.h"

using namespace std;

DMDColor::DMDColor() {
	c.value = 0;
		
}

DMDColor::DMDColor(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t alpha1) {
	c.cols.r = r1;
	c.cols.g = g1;
	c.cols.b = b1;
	c.cols.alpha = alpha1;
}

DMDColor::DMDColor(uint32_t colors) {
	c.value = colors;
}

bool DMDColor::matches(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t alpha1) {
	return ((r1 == c.cols.r) && (g1 == c.cols.g) && (b1 == c.cols.b) && (alpha1 == c.cols.alpha));
}

bool DMDColor::matches(uint8_t r1, uint8_t g1, uint8_t b1) {
	return ((r1 == c.cols.r) && (g1 == c.cols.g) && (b1 == c.cols.b));
}

bool DMDColor::matches(DMDColor color, bool ignore_alpha) {
	if ((color.c.cols.r == c.cols.r) && (color.c.cols.g == c.cols.g) && (color.c.cols.b == c.cols.b)) {
		if (ignore_alpha) {
			return true;
		}
		else {
			return color.c.cols.alpha == c.cols.alpha;
		}
	}
	return false;
}

DMDPalette::DMDPalette(int size, int bitsperpixel, string name) {
	assert(size >= (1 << bitsperpixel));

	this->size = size;
	this->bitsperpixel = bitsperpixel;
	colors = new DMDColor[size];
	this->name = name;
}

DMDPalette::DMDPalette(uint32_t* colors, int size, int bitsperpixel, string name1)
{
	assert(size >= (1 << bitsperpixel));

	this->size = size;
	this->colors = new DMDColor[size];
	for (int i = 0; i < size; i++) {
		this->colors[i].c.value = colors[i];
	}

	this->name = name;
}

DMDPalette::~DMDPalette() {
	delete colors;
}

int DMDPalette::find(uint32_t color, bool ignore_alpha) {
	for (int i = 0; i < size; i++) {
		if (colors[i].matches(color, ignore_alpha)) {
			return i;
		}
	}
	return -1;
}

int DMDPalette::find(uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; i < size; i++) {
		if (colors[i].matches(r,g,b)) {
			return i;
		}
	}
	return -1;
}

bool DMDPalette::matches(RGBBuffer* buf) {
	for (int i = 0; i < buf->len; i++) {
		rgb_t v = buf->data[i];
		bool color_found = false;

		for (int ci = 0; ci < size; ci++) {
			if (colors[ci].matches(v.r, v.g, v.b)) {
				color_found = true;
				break;
			}
		}

		if (!(color_found)) {
			return false;
		}
	}
	return true;
}



DMDPalette* find_matching_palette(vector<DMDPalette*> palettes, RGBBuffer* buf)
{
	for (const auto& palette : palettes) {
		if (palette->matches(buf)) {
			return palette;
		}
	}

	return NULL;
}

vector<DMDPalette*> default_palettes() {
	vector<DMDPalette*> res = vector<DMDPalette*>();

	uint32_t pd_4_orange_mask_data[] = {
		0x00000000, 0x11050000, 0x22090000, 0x330e0000,
		0x44120000, 0x55170000, 0x661C0000, 0x77200000,
		0x88000000, 0x99000000, 0xaa000000, 0xbb000000,
		0xcc370000, 0xdd3c0000, 0xee400000, 0xff450000,
		0xfd00fd00 };
	DMDPalette* pd_4_orange_mask = new DMDPalette(pd_4_orange_mask_data, 17, 4, "pd_4_orange_mask");

	res.push_back(pd_4_orange_mask);

	return res;
}

