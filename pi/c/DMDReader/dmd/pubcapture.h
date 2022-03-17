#pragma once

#include "dmdframe.h"
#include "frameprocessor.h"

class PubCapture : DMDFrameProcessor {

public:

	PubCapture(int bitsperpixel, string directory);
	~PubCapture();
	virtual void process(DMDFrame*);


};