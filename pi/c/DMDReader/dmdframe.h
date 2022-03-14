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

	uint16_t columns;
	uint16_t rows;
	uint16_t bitsperpixel;
	uint8_t* data;

public:

	DMDFrame(uint16_t columns1 = 0, uint16_t rows1 = 0, uint16_t bitsperpixel1 = 0, char* data1 = NULL);
	~DMDFrame();

	PIXVAL getPixel(uint16_t x, uint16_t y);

	bool read_from_stream(std::ifstream& fis);

	bool same_size(DMDFrame f2);
	bool equals_fast(DMDFrame f2);

	string str();


private:

	void recalc_checksum();

	void init_mem(char* data1 = NULL);

	// cache some stuff
	uint16_t datalen;
	uint16_t rowlen;
	uint16_t bitmask;
	uint32_t checksum; // uses for fast equality check
};
