#pragma once

#include "dmdframe.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/log/trivial.hpp>

class DMDFrameProcessor {

public:
	virtual void process(DMDFrame*);

	virtual bool configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source);
};