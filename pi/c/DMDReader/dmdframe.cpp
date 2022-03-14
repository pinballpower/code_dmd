
#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "util/crc32.h"

#include "dmdframe.h"

DMDFrame::DMDFrame(uint16_t columns1, uint16_t rows1, uint16_t bitsperpixel1, char* data1)
	{
		columns = columns1;
		rows = rows1;
		bitsperpixel = bitsperpixel1;
		data = NULL;
		checksum = 0;
		bitmask = 0;

		this->init_mem(data1);
	}


DMDFrame::~DMDFrame() {

		if (data) {
			free(data);
		}
	}

PIXVAL DMDFrame::getPixel(uint16_t x, uint16_t y) {
		uint16_t offset = y * rowlen + x / bitsperpixel;
		uint16_t pixoffset = 8 - (x % bitsperpixel);
		return (data[offset] >> pixoffset) & bitmask;
	}

bool DMDFrame::read_from_stream(std::ifstream& fis)
	{
		if ((!fis.good()) || fis.eof()) {
			return false;
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
			return false;
		}

		return true;
	}

	bool DMDFrame::same_size(DMDFrame f2) {
		return ((columns = f2.columns) && (rows = f2.rows) && (bitsperpixel = f2.bitsperpixel));
	}

	bool DMDFrame::equals_fast(DMDFrame f2) {
		if (this->same_size(f2)) {
			return checksum == f2.checksum;
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

	void DMDFrame::init_mem(char* data1) {
		rowlen = columns * bitsperpixel / 8;
		datalen = rowlen * rows;
		bitmask = 0xff << (8 - bitsperpixel);

		if (datalen) {

			if (data) {
				free(data);
				data = NULL;
			}

			data = (uint8_t*)malloc(datalen);

			if (data1) {
				memcpy_s(data, datalen, data1, datalen);
			}
			else if (data) {
				memset(data, 0, datalen);
			}

			recalc_checksum();
		}
		else {
			data = NULL;
		}
	}

