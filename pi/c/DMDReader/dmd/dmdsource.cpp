#include "dmdsource.h"

DATDMDSource::DATDMDSource()
{
}

DATDMDSource::DATDMDSource(string filename)
{
	read_file(filename);
}

DATDMDSource::~DATDMDSource()
{
   // TODO: clear
}

void DATDMDSource::read_file(string filename)
{
	ifstream df(filename.c_str(), ios::in | ios::binary);
	if (!df) {
		return;
	}

	df.exceptions(ifstream::failbit | ifstream::badbit);

	int rc = 0;
	while (rc == 0) {
		DMDFrame* frame = new DMDFrame();
		rc = frame->read_from_stream(df);
		if (rc == 0) {
			frames.push(frame);
		}
	}
}

DMDFrame* DATDMDSource::next_frame(bool blocking)
{
	DMDFrame* res = frames.front();
	frames.pop();
	return res;
}

bool DATDMDSource::finished()
{
	return frames.empty();
}

bool DATDMDSource::frame_ready()
{
	return (!finished());
}

DMDFrame* DMDSource::next_frame(bool blocking)
{
	return nullptr;
}

bool DMDSource::finished()
{
	return true;
}

bool DMDSource::frame_ready()
{
	return false;
}
