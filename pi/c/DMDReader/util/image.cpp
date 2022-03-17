#include <cassert>

#include "image.h"

RGBBuffer::RGBBuffer()
{
	len = width = height = 0;
	data = NULL;
}

RGBBuffer::RGBBuffer(int width, int height)
{
	assert((width >= 0) && (height >= 0));

	len = width * height;
	this->width = width;
	this->height = height;
	data = new rgb_t[len];
}

RGBBuffer::~RGBBuffer()
{
	delete data;
	data = NULL;
}
