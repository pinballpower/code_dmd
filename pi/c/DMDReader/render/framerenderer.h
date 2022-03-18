#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/log/trivial.hpp>

#include "../dmd/dmdframe.h"
#include "../dmd/color.h"

class FrameRenderer
{
public:
	FrameRenderer();
	~FrameRenderer();

	virtual void render_frame(DMDFrame* f);

	virtual bool configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_renderer);
};