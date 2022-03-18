#pragma once

#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/log/trivial.hpp>

#include "dmdframe.h"
#include "frameprocessor.h"

class PubCapture : DMDFrameProcessor {

public:

	PubCapture();
	~PubCapture();
	virtual bool configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source);
	bool load_triggers(int bitsperpixel, string directory, DMDPalette* palette = NULL);

	virtual void process(DMDFrame*);

private:
	DMDPalette* palette;
	map<int, MaskedDMDFrame*> trigger_frames;
};