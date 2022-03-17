#pragma once

#include <map>

#include "dmdframe.h"
#include "frameprocessor.h"

class PubCapture : DMDFrameProcessor {

public:

	PubCapture(int bitsperpixel, string directory, DMDPalette* palette=NULL);
	~PubCapture();
	virtual void process(DMDFrame*);

private:
	DMDPalette* palette;
	map<int, MaskedDMDFrame*> trigger_frames;
};