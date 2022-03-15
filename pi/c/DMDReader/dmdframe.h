#pragma once

#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "util/crc32.h"

#define  PIXVAL uint8_t  // we don't deal with more than 8 bits/pixel

using namespace std;

class DMDFrame {

private:

	int columns;
	int rows;
	int bitsperpixel;
	uint8_t* data;

public:

	DMDFrame(int columns1 = 0, int rows1 = 0, int bitsperpixel1 = 0, uint8_t* data1 = NULL);
	~DMDFrame();

	PIXVAL getPixel(int x, int y);

	bool read_from_stream(std::ifstream& fis);

	bool same_size(DMDFrame f2);
	bool equals_fast(DMDFrame f2);

	int get_width();
	int get_height();
	int get_bitsperpixel();
	uint8_t get_pixelmask();

	uint8_t* get_data();

	string str();


private:

	void recalc_checksum();

	void init_mem(uint8_t* data1 = NULL);

	// cache some stuff
	uint16_t datalen;
	uint16_t rowlen;
	uint16_t pixel_mask;
	uint32_t checksum; // uses for fast equality check
};
