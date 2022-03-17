#pragma once

#include <cstdint>

typedef struct rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} rgb_t;


class RGBBuffer {

public:
	rgb_t* data;
	int len;
	int width;
	int height;


	RGBBuffer();
	RGBBuffer(int width, int height);
	~RGBBuffer();
};