#include "../dmd/dmdsource.h"
#include "../dmd/pubcapture.h"
#include "../dmd/frameprocessor.h"
#include "../render/framerenderer.h"
#include "../render/raylibrenderer.h"

DMDSource* createSource(string name) {

	if (name == "file") {
		return (DMDSource*)(new DATDMDSource());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "source name " << name << "unknown";
		return NULL;
	}

}

DMDFrameProcessor* createProcessor(string name) {

	if (name == "pubcapture") {
		return (DMDFrameProcessor*)(new PubCapture());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "processor name " << name << "unknown";
		return NULL;
	}

}


FrameRenderer* createRenderer(string name) {
	if (name == "raylib") {
		return (FrameRenderer*)(new RaylibRenderer());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "renderer name " << name << "unknown";
		return NULL;
	}

}
