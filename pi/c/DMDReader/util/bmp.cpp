// simple BMP file reader fbased on https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file

#include <cstdio>
#include <cstdint>

unsigned char* ReadBMP(char* filename, int* width1, int* height1)
{

    *width1 = 0;
    *height1 = 0;

    int i;
    FILE* f = fopen(filename, "rb");

    if (f == NULL)
        throw "Argument Exception";

    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int row_padded = (width * 3 + 3) & (~3);
    uint8_t* data = new uint8_t[row_padded];
    uint8_t tmp;

    for (int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int j = 0; j < width * 3; j += 3)
        {
            // Convert (B, G, R) to (R, G, B)
            tmp = data[j];
            data[j] = data[j + 2];
            data[j + 2] = tmp;

            // cout << "R: " << (int)data[j] << " G: " << (int)data[j + 1] << " B: " << (int)data[j + 2] << endl;
        }
    }

    fclose(f);

    // return width, height and data
    *height1 = height;
    *width1 = width;
    return data;
}