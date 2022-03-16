
#include <cstdint>

union colorUnion{
	uint32_t value;
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t alpha;
	} cols;
};

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

	DMDPalette(int size1);
	DMDPalette(uint32_t* colors1, int size);
	~DMDPalette();

	int find(uint32_t color, bool ignore_alpha=true);
//	int find(DMDColor color, bool ignore_alpha=true);

};
