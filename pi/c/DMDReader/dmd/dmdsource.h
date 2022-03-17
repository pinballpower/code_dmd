#pragma once

#include <queue>
#include "dmdframe.h"

using namespace std;

class DMDSource {

public:
	virtual DMDFrame* next_frame(bool blocking = true);
	virtual bool finished();
	virtual bool frame_ready();
	virtual int configure_from_json();
};

class DATDMDSource : DMDSource {

public:

	DATDMDSource();
	DATDMDSource(string filename);
	~DATDMDSource();

	void read_file(string filename);

	virtual DMDFrame* next_frame(bool blocking = true);

	virtual bool finished();
	virtual bool frame_ready();

private:

	queue<DMDFrame*> frames;
};