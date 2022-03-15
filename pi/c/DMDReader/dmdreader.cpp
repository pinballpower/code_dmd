// DMDReader.cpp: Definiert den Einstiegspunkt für die Anwendung.
//
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>

#include "dmdframe.h"
#include "dmddata.h"
#include "DMDReader.h"
#include "util/crc32.h"

#include "render/framerenderer.h"
#include "render/raylibrenderer.h"

using namespace std;

std::vector<DMDFrame*> readfile(string name) {

	std::vector<DMDFrame*> res;

	ifstream df(name.c_str(), ios::in | ios::binary);
	if (!df) {
		return res;
	}

	df.exceptions(ifstream::failbit | ifstream::badbit);
	DMDFrame frame = DMDFrame();

	int rc = 0;

	while (rc==0) {
		rc = frame.read_from_stream(df);
		if (rc==0) {
			cout << frame.str() << "\n";
		};

		DMDFrame* framegray = frame.to_gray8();
		cout << framegray->str() << "\n";

		res.push_back(framegray);
	}

	return res;
}

int main()
{
	string datadir = "../../../../../samples/";

	RaylibRenderer rr = RaylibRenderer(128 * 11,32 * 11,5,1,1);

	cout << std::filesystem::current_path() << endl;
	std::vector<DMDFrame*> frames=readfile(datadir+"spiimage-ghostbusters3.dat");
	// std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	DMDFrame frame = DMDFrame();

	MaskedDMDFrame mframe = MaskedDMDFrame();
	mframe.read_from_bmp(datadir+"ghostbusters-2.bmp");

	int i = 0;
	for (auto& f : frames) {
		i++;
		rr.showImage(f);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (mframe.matches(f)) {
			cout << f->str() << "\n";
		}
		//	rr.showImage((DMDFrame*)&mframe);
		//}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10000));

	return 0;
}
