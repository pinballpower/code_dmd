#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "../util/endian.h"
#include "../util/image.h"

using namespace std;

#if ENDIAN == LITTLE
union colorUnion{
	uint32_t value;
	struct {
		uint8_t alpha;
		uint8_t b;
		uint8_t g;
		uint8_t r;
	} cols;
};
#endif

class DMDColor {

public:
	colorUnion c;

	DMDColor();
	DMDColor(uint32_t colors);
	DMDColor(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t alpha1 = 0);

	bool matches(uint8_t r1, uint8_t g1, uint8_t b1);
	bool matches(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t alpha1);
	bool matches(DMDColor c, bool ignore_alpha = true);

};

class DMDPalette {

public:
	DMDColor* colors;
	int size;
	int bitsperpixel;
	string name;

	DMDPalette(int size, int bitsperpixel, string name="");
	DMDPalette(uint32_t* colors, int size, int bitsperpixel, string name="");
	~DMDPalette();

	int find(uint32_t color, bool ignore_alpha=true);
	int find(uint8_t r, uint8_t g, uint8_t b);

};

bool palette_matches(DMDPalette* palette, RGBBuffer* buf);
DMDPalette* find_matching_palette(vector<DMDPalette*> palettes, RGBBuffer* buf);
vector<DMDPalette*> default_palettes();
