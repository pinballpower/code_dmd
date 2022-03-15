// DMDReader.cpp: Definiert den Einstiegspunkt für die Anwendung.
//
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>

#include "dmdframe.h"
#include "dmddata.h"
#include "DMDReader.h"
#include "util/crc32.h"

#include "render/framerenderer.h"
#include "render/raylibrenderer.h"

using namespace std;

int readfile(string name) {
	ifstream df(name.c_str(), ios::in | ios::binary);
	if (!df) {
		cout << "Cannot open file!" << endl;
		return 1;
	}

	df.exceptions(ifstream::failbit | ifstream::badbit);
	DMDFrame frame = DMDFrame();

	bool ok = true;

	RaylibRenderer rr = RaylibRenderer();
	

	while (ok) {
		ok = frame.read_from_stream(df);
		if (ok) {
			cout << frame.str() << "\n";
		};

		rr.showImage(&frame);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}

int main()
{
	readfile("C:\\Users\\matuschd\\devel\\pico\\code_dmd\\pi\\samples\\spiimage-ghostbusters.dat");

	DMDFrame f = DMDFrame();

	return 0;
}
