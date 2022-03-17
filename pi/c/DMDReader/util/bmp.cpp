// simple BMP file reader fbased on https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <fstream>

#include "bmp.h"
#include "image.h"

using namespace std;

RGBBuffer* read_BMP(std::string filename)
{
    ifstream is;
    is.exceptions(ifstream::failbit | ifstream::badbit);
    is.open(filename, ios::binary);

    uint8_t info[54];
    is.read((char*)info, sizeof(info)); // read the 54-byte header

    // check if it is the format with the 54 byte header
    if (info[14] != 40) {
        cerr << "Can't read " << filename << ", only BITMAPINFOHEADER supported, but type is " << info[14] << "\n";
        return NULL;
    }
    
    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    if ((width <= 0) || (height <= 0)) {
        cerr << "Can't read " << filename << ", invalid dimensions.\n";
        return NULL;
    }

    int row_padded = (width * 3 + 3) & (~3);
    int bytesperline = 3 * width;

    uint8_t* linedata = new uint8_t[row_padded];
    RGBBuffer* res = new RGBBuffer(width,height);

    rgb_t* dst;

    for (int i = 0; i < height; i++)
    {
        is.read((char*)linedata, row_padded);
        dst = res->data + ((height-1-i) * width);

        for (int j = 0; j < width * 3; j += 3, dst ++)
        {
            // Convert (B, G, R) to (R, G, B)
            (*dst).r = linedata[j + 2];
            (*dst).g = linedata[j + 1];
            (*dst).b = linedata[j];
        }
    }

    delete[] linedata;

    is.close();

    // return width, height and data
    return res;
}