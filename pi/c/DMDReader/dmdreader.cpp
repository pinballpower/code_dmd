// DMDReader.cpp: Definiert den Einstiegspunkt für die Anwendung.
//
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>

#include "DMDReader.h"

#include "dmd/dmdframe.h"
#include "dmd/dmddata.h"
#include "dmd/pubcapture.h"
#include "dmd/dmdsource.h"
#include "render/framerenderer.h"
#include "render/raylibrenderer.h"

using namespace std;



int main()
{
	string datadir = "../../../../../samples/";

	// Pubcapture test
	PubCapture pubcapture = PubCapture(4, datadir + "gbpub");


	// find the correct palette for a file
	vector<DMDPalette*> palettes = default_palettes();


	RaylibRenderer rr = RaylibRenderer(128 * 11,32 * 11,5,1,8);

	cout << std::filesystem::current_path() << endl;

	DATDMDSource src = DATDMDSource(datadir + "spiimage-ghostbusters3.dat");


	int i = 0;
	while (! src.finished()) {

		DMDFrame *f = src.next_frame(true);

		i++;
		if (i >= 461) {

			rr.showImage(f);
			cout << i << "\n";
			pubcapture.process(f);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		delete f;
	
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10000));

	return 0;
}
