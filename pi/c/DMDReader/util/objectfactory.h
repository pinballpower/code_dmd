#pragma once

/*
* A simple object factory. As the number of classes is limited, all mappings are hardcoded.
*/

#include "../dmd/dmdsource.h"
#include "../dmd/pubcapture.h"
#include "../dmd/frameprocessor.h"
#include "../render/framerenderer.h"
#include "../render/raylibrenderer.h"

template<typename T> Base* createInstanceJSON() { return new T; }

typedef std::map<std::string, Base* (*)()> objectmapper_t;

objectmapper_t mapper_sources;
mapper_sources["null"] = &createInstance<DMDSource>;
mapper_sources["file"] = &createInstance<DATDMDSource>;

objectmapper_t mapper_processors;
mapper_sources["null"] = &createInstance<DMDFrameProcessor>;
mapper_sources["pubcapture"] = &createInstance<PubCapture>;

objectmapper_t mapper_renderers;
mapper_sources["null"] = &createInstance<FrameRenderer>;
mapper_sources["raylib"] = &createInstance<RaylibRenderer>;

objectmapper_t mapper_colorisations;


