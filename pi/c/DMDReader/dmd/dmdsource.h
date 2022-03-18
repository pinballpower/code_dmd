#pragma once

#include <queue>

#include <boost/property_tree/ptree.hpp>
#include <boost/log/trivial.hpp>

#include "dmdframe.h"

using namespace std;

typedef struct SourceProperties {
	int width;
	int height;
	int bitsperpixel;
} SourceProperties;

class DMDSource {

public:
	virtual DMDFrame* next_frame(bool blocking = true);
	virtual bool finished();
	virtual bool frame_ready();
	virtual bool configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source);

	virtual void get_properties(SourceProperties* p);
};

class DATDMDSource : DMDSource {

public:

	DATDMDSource();
	DATDMDSource(string filename);
	~DATDMDSource();

	bool read_file(string filename);

	virtual DMDFrame* next_frame(bool blocking = true);

	virtual bool finished();
	virtual bool frame_ready();

	virtual bool configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source);

	virtual void get_properties(SourceProperties* p);

private:

	queue<DMDFrame*> frames;
};