
#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "../util/crc32.h"
#include "../util/bmp.h"

#include "dmdframe.h"
#include "../util/numutils.h"

DMDFrame::DMDFrame(int columns1, int rows1, int bitsperpixel1, uint8_t* data1)
{
	columns = columns1;
	rows = rows1;
	bitsperpixel = bitsperpixel1;
	data = NULL;
	checksum = 0;
	pixel_mask = 0;

	this->init_mem(data1);
}


DMDFrame::~DMDFrame() {
	if (data) {
		delete[] data;
	}
}

PIXVAL DMDFrame::getPixel(int x, int y) {
	int offset = y * rowlen + x / bitsperpixel;
	int pixoffset = 8 - (x % bitsperpixel);
	return (data[offset] >> pixoffset) & pixel_mask;
}

int DMDFrame::read_from_stream(std::ifstream& fis)
{
	if ((!fis.good()) || fis.eof()) {
		return -1;
	}

	uint8_t header[8];
	try {
		fis.read((char*)header, 8);
		rows = (header[0] << 8) + header[1];
		columns = (header[2] << 8) + header[3];
		bitsperpixel = (header[6] << 8) + header[7];
		this->init_mem(NULL);

		fis.read((char*)data, datalen);
		recalc_checksum();
	}
	catch (std::ios_base::failure) {
		return -1;
	}

	return 0;
}

bool DMDFrame::same_size(DMDFrame* f2) {
	return ((columns = f2->columns) && (rows = f2->rows) && (bitsperpixel = f2->bitsperpixel));
}

bool DMDFrame::equals_fast(DMDFrame* f2) {
	if (this->same_size(f2)) {
		return checksum == f2->checksum;
	}
	else {
		return false;
	}
}

std::string DMDFrame::str() {
	char cs[8];
	snprintf(cs, sizeof(cs), "%08x", checksum);
	return "DMDFrame(" + std::to_string(columns) + "x" + std::to_string(rows) + "," + std::to_string(bitsperpixel) + "bpp, checksum=" + cs + ")";
}

void DMDFrame::recalc_checksum() {
	if (data && datalen) {
		checksum = crc32buf(data, datalen);
	}
	else {
		checksum = 0;
	};
}

void DMDFrame::init_mem(uint8_t* data1) {
	rowlen = columns * bitsperpixel / 8;

	// make sure it's 32bit aligned (usually it should be, but just to be sure)
	datalen = roundup_4(rowlen * rows);

	pixel_mask = 0xff >> (8 - bitsperpixel);

	if (datalen) {

		if (data) {
			delete[] data;
			data = NULL;
		}

		data = new uint8_t[datalen];

		if (data1) {
			memcpy_s(data, datalen, data1, datalen);
		}
		else {
			memset(data, 0, datalen);
		}

		recalc_checksum();
	}
	else {
		data = NULL;
	}
}

inline uint8_t DMDFrame::next_pixel(uint8_t **buf, int *pixel_bit) {
	*pixel_bit -= bitsperpixel;
	if (*pixel_bit < 0) {
		*pixel_bit += 8;
		(*buf)++;
	}

	return ((**buf >> *pixel_bit) & pixel_mask);
}


DMDFrame* DMDFrame::to_gray8() {
	DMDFrame* res = new DMDFrame(columns, rows, 8);

	int pixel_bit = 8;
	uint8_t *src = data;
	uint8_t *dst = res->data;

	int shift = 8 - bitsperpixel;

	for (int pixel = 0; pixel < rows*columns; pixel++) {
		uint8_t pv = next_pixel(&src, &pixel_bit);

		// copy pixel
		*dst = pv << shift;
		dst++;
	}

	res->recalc_checksum();

	return res;
}

DMDFrame* DMDFrame::to_gray1(int threshold) {
	DMDFrame* res = new DMDFrame(columns, rows, 1);

	int src_bit = 8;
	int dst_bit = 8;

	uint8_t* src = data;
	uint8_t* dst = res->data;
	uint8_t nv = 0;

	int shift = 8 - bitsperpixel;

	for (int pixel = 0; pixel < rows * columns; pixel++) {
		uint8_t pv = next_pixel(&src, &src_bit);


		dst_bit -= 1;
		nv <<= 1;
		if (pv > threshold) {
			nv += 1;
		}
		if (dst_bit <= 0) {
			*dst = nv;
			nv = 0;
			dst_bit += 8;
			dst++;
		}
	}

	res->recalc_checksum();

	return res;
}




int DMDFrame::get_width() {
	return columns;
}

int DMDFrame::get_height() {
	return rows;
}

uint8_t* DMDFrame::get_data() {
	return data;
}

uint8_t DMDFrame::get_pixelmask() {
	return pixel_mask;
}

int DMDFrame::get_bitsperpixel() {
	return bitsperpixel;
}

MaskedDMDFrame::MaskedDMDFrame() {
	mask = NULL;
}

MaskedDMDFrame::~MaskedDMDFrame() {
	if (mask) {
		delete[] mask;
	}
}

bool MaskedDMDFrame::matches(DMDFrame* frame) {

	if ((frame->get_bitsperpixel() != DMDFrame::bitsperpixel) ||
		(frame->get_width() != DMDFrame::columns) ||
		(frame->get_height() != DMDFrame::rows)) 
	{
		return false;
	}

	/* Everything is 4-byte aligned, therefore we'll use 32-bit operators */
	uint32_t* orig = (uint32_t * )frame->get_data();
	uint32_t* to_compare = (uint32_t*) DMDFrame::data;
	uint32_t* msk = (uint32_t*)mask;

	for (int i = 0; i < DMDFrame::datalen / 4; i++) {
		if ((*orig & *msk) != *to_compare) {
			return false;
		}
		*orig++;
		*msk++;
		*to_compare++;
	}

	return true;
}

int MaskedDMDFrame::read_from_bmp(string filename, int grayoffset, int maskoffset) {

	int width = 0;
	int height = 0;
	uint8_t* bmp_data = read_BMP(filename, &width, &height);

	if ((width <= 0) || (height <= 0)) {
		return -1;
	}

	// Initialize memory
	DMDFrame::columns = width;
	DMDFrame::rows = height;
	DMDFrame::bitsperpixel = 8;
	init_mem();
	if (mask) {
		delete[] mask;
	};
	mask = new uint8_t[width * height];


	// Mask calculations
	int mask_x1, mask_x2, mask_y1, mask_y2;
	mask_x1 = columns + 1;
	mask_x2 = -1;
	mask_y1 = rows + 1;
	mask_y2 = -1;

	uint8_t* src = bmp_data;
	uint8_t* dst = DMDFrame::data;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (src[maskoffset]) {
				if (x < mask_x1) {
					mask_x1 = x;
				}
				if (x > mask_x2) {
					mask_x2 = x;
				}
				if (y < mask_y1) {
					mask_y1 = y;
				}
				if (y > mask_y2) {
					mask_y2 = y;
				}
			}

			*dst = src[grayoffset];
	
			src += 3;
			dst++;

		}
	}

	// Masking
	dst = data;
	uint8_t* msk = mask;
	if ((mask_x1 <= mask_x2) && (mask_y1 <= mask_y2)) {
		// mask rectangle found
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if ((x <= mask_x1) || (x >= mask_x2) || (y <= mask_y1) || (y >= mask_y2)) {
					*dst = 0;
					*msk = 0;
				}
				else {
					*msk = 0xff;
				}
				// next pixel
				dst++;
				msk++;
			}
		}
	}

	DMDFrame::recalc_checksum();

	delete[] bmp_data;

	return 0;
}