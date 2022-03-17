#include <filesystem>
#include <regex>
#include <iostream>
#include <map>
#include <iterator>

#include "pubcapture.h"
#include "../util/image.h"
#include "../util/bmp.h"

using namespace std;


PubCapture::PubCapture(int bitsperpixel, string directory, DMDPalette* palette)
{
    this->palette = palette;
    trigger_frames = map<int, MaskedDMDFrame*>();

    regex file_expr("([0-9]+).bmp");

    std::filesystem::path folder(directory);
    if (!std::filesystem::is_directory(folder))
    {
        throw std::runtime_error(folder.string() + " is not a folder");
    }

    map<int, RGBBuffer*> rgbdata;

    // find highest ID
    int max_index = 0;
    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        const auto full_name = entry.path().string();

        if (entry.is_regular_file())
        {
            const auto filename = entry.path().filename().string();

            smatch match;
            if (regex_search(filename, match, file_expr)) {
                int i = stoi(match.str(1));
                if (i > max_index) { max_index = i; };

                RGBBuffer *buff = read_BMP(full_name);
                rgbdata.insert(pair<int, RGBBuffer*>(i, buff));

                cout << "loaded " << filename << "\n";

            }
        }
    }

    // Find the correct palette for these if it's not given
    if (!this->palette) {
        vector<DMDPalette*> palettes = default_palettes();
        for (const auto& p : palettes) {

            cout << "Checking " << p->name << "\n";
            bool matches = true;

            map<int, RGBBuffer*>::iterator itr;
            for (itr = rgbdata.begin(); itr != rgbdata.end(); ++itr) {
                RGBBuffer* buff = itr->second;
                cout << buff << '\n';

                if (!p->matches(buff)) {
                    break;
                    matches = false;
                }
            }
            if (matches) {
                this->palette = p;
                break;
            }
        }
    }

    if (!this->palette) {
        cerr << "Couldn't find matching palette, aborting" << "\n";
    }

    // create masked frames
    map<int, RGBBuffer*>::iterator itr;
    for (itr = rgbdata.begin(); itr != rgbdata.end(); ++itr) {
        int i = itr->first;
        RGBBuffer* buf = itr->second;

        MaskedDMDFrame* mf = new MaskedDMDFrame();
        mf->read_from_rgbimage(buf, this->palette, bitsperpixel);
        trigger_frames.insert(pair<int, MaskedDMDFrame*>(i, mf));
    }


    if (max_index == 0) {
        cerr << "No pubcapture files found in " << directory << "\n";
    }

}

PubCapture::~PubCapture()
{
    trigger_frames.clear();
}

void PubCapture::process(DMDFrame* f)
{
    // check all maksedframes if one matches
    map<int, MaskedDMDFrame*>::iterator itr;
    for (itr = trigger_frames.begin(); itr != trigger_frames.end(); ++itr) {
        int i = itr->first;
        MaskedDMDFrame* mf = itr->second;

        if (mf->matches(f)) {
            cout << "found pubcapture match: " << i << "\n";
        }
    }


}

