#include <filesystem>
#include <regex>
#include <iostream>

#include "pubcapture.h"

using namespace std;


PubCapture::PubCapture(int bitsperpixel, string directory)
{
    regex file_expr("([0-9]+).bmp");

    std::filesystem::path folder(directory);
    if (!std::filesystem::is_directory(folder))
    {
        throw std::runtime_error(folder.string() + " is not a folder");
    }

    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        const auto full_name = entry.path().string();

        if (entry.is_regular_file())
        {
            const auto filename = entry.path().filename().string();

            if (regex_match(filename, file_expr)) {
                cout << filename;

            }
        }
    }
}

PubCapture::~PubCapture()
{
}

void PubCapture::process(DMDFrame*)
{
}

