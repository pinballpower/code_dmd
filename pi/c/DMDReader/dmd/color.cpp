#include <cstdint>
#include <iostream>
#include <vector>

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

DMDPalette::DMDPalette(int size1, string name1) {
	size = size1;
	colors = new DMDColor[size1];
	name = name1;
}

DMDPalette::DMDPalette(uint32_t* colors1, int size1, string name1)
{
	size = size1;
	colors = new DMDColor[size];
	for (int i = 0; i < size; i++) {
		colors[i].c.value = colors1[i];
	}

	name = name1;
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


DMDPalette* find_matching_palette(vector<DMDPalette*> palettes, rgb *buf, int buflen) {

	for (auto palette = std::begin(palettes); palette != std::end(palettes); ++palette) {

		bool palette_ok = true;

		for (int i = 0; i < buflen; i++) {
			rgb_t v = buf[i];
			bool color_found = false;

			for (int ci = 0; ci < (*palette)->size; ci++) {
				if ((*palette)->colors[ci].matches(v.r, v.g, v.b)) {
					color_found = true;
					break;
				}
			}

			if (!(color_found)) {
				cout << "Can't find color " << v.r + 0 << "," << v.g + 0 << "," << v.b + 0 << " in palette " << (*palette)->name << "\n";
				palette_ok=false;
				break;
			}
		}

		if (palette_ok) {
			return (*palette);
		};
	}

	return NULL;
}

vector<DMDPalette*> default_palettes() {
	vector<DMDPalette*> res = vector<DMDPalette*>();

	DMDPalette *p1 = new DMDPalette(pd_2_orange_mask, 17, "pd_2_orange_mask");
	res.push_back(p1);

	return res;
}

