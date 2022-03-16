#include <cstdint>
#include "color.h"

DMDColor::DMDColor() {
		
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

DMDPalette::DMDPalette(int size1) {
	int size = size1;
	colors = new DMDColor[size1];
}

//DMDPalette::DMDPalette(DMDColor* colors1, int size) {
//	colors = new DMDColor[size];
//	for (int i = 0; i < size; i++) {
//		colors[i] = colors1[i];
//	}
//}

DMDPalette::DMDPalette(uint32_t* colors1, int size)
{
	colors = new DMDColor[size];
	for (int i = 0; i < size; i++) {
		colors[i].c.value = colors1[i];
	}
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
