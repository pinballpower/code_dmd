#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "../util/endian.h"

using namespace std;

static uint32_t pd_4_gray[16] =
{ 0x00000000, 0x10101000, 0x20202000, 0x30303000,
  0x40404000, 0x50505000, 0x60606000, 0x70707000,
  0x80808000, 0x90909000, 0xa0a0a000, 0xb0b0b000,
  0xc0c0c000, 0xd0d0d000, 0xe0e0e000, 0xf0f0f000 };

// It seems that the 0x88, 0x99, 0xaa and 0xbb lebels are not used
static uint32_t pd_2_orange_mask[9] =
{ 0x00000000, 0x11050000, 0x44120000, 0xf0780000, 0xff450000, 0x80400000, 0xff000000, 

  0xfd00fd00, 0xff582000
};

// It seems that the 0x88, 0x99, 0xaa and 0xbb lebels are not used
static uint32_t pd_4_orange_mask[17] =
{ 0x00000000, 0x11050000, 0x22090000, 0x330e0000,
  0x44120000, 0x55170000, 0x661C0000, 0x77200000,
  0x88000000, 0x99000000, 0xaa000000, 0xbb000000,
  0xcc370000, 0xdd3c0000, 0xee400000, 0xff450000,
  0xfd00fd00
 };

static uint32_t pd_4_red_mask[17] =
{ 0x00000000, 0x11000000, 0x22000000, 0x33000000,
  0x44000000, 0x55000000, 0x66000000, 0x77000000,
  0x88000000, 0x99000000, 0xaa000000, 0xbb000000,
  0xcc000000, 0xdd000000, 0xee400000, 0xff000000,
  0xfd00fd00
};

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

typedef struct rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} rgb_t;

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
	string name;

	DMDPalette(int size1, string name1="");
	DMDPalette(uint32_t* colors1, int size, string name1="");
	~DMDPalette();

	int find(uint32_t color, bool ignore_alpha=true);
	int find(uint8_t r, uint8_t g, uint8_t b);

};

DMDPalette* find_matching_palette(vector<DMDPalette*> palettes, rgb* buf, int buflen);
vector<DMDPalette*> default_palettes();
