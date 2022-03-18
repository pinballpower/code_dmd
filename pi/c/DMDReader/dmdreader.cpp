// DMDReader.cpp: Definiert den Einstiegspunkt für die Anwendung.
//
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <map>

#include <boost/log/trivial.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "DMDReader.h"
#include "dmd/dmdframe.h"
#include "dmd/dmddata.h"
#include "dmd/pubcapture.h"
#include "dmd/dmdsource.h"
#include "render/framerenderer.h"
#include "render/raylibrenderer.h"
#include "util/objectfactory.h"

using namespace std;

DMDSource* source = NULL;
vector<DMDFrameProcessor*> processors = vector<DMDFrameProcessor*>();

bool read_config(string filename) {

	int i = 0;

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(filename, pt);

	//
	// General
	//
	boost::property_tree::ptree pt_general = pt.get_child("general");
	if ((pt.get("general.type","") == "spike") || (pt.get("general.type","") == "spike1")) {
		pt.put("general.bitsperpixel", 4);
	}

	if ((pt.get("general.type", "") == "wpc")) {
		pt.put("general.bitsperpixel", 2);
	}

	if ((pt.get("general.type", "") == "whitestar")) {
		pt.put("general.bitsperpixel", 4);
	}

	//
	// Sources
	//
	BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, pt.get_child("source")) {
		if (i > 0) {
			BOOST_LOG_TRIVIAL(info) << "ignoring " << v.first << " only a single source is supported";
		}
		else {
			source = createSource(v.first);
			if (source) {
				if (source->configure_from_ptree(pt_general, v.second)) {
					BOOST_LOG_TRIVIAL(info) << "successfully initialized input type " << v.first;
				}
				else {
					BOOST_LOG_TRIVIAL(error) << "couldn't initialise source " << v.first;
					return false;
				}
			}
			else {
				BOOST_LOG_TRIVIAL(error) << "don't know input type " << v.first;
				return false;
			}
		}
		i++;
	}

	// 
	// Sanity checks
	//
	int bpp_configured = pt.get("general.bitsperpixel", 0);
	SourceProperties sourceprop = { 0,0,0 };
	source->get_properties(sourceprop);

	if (!(bpp_configured)) {
		pt.put("general.bitsperpixel",sourceprop.bitsperpixel);
	} else if (sourceprop.bitsperpixel && (bpp_configured != sourceprop.bitsperpixel)) {

		BOOST_LOG_TRIVIAL(error) << "bits/pixel configured=" << bpp_configured << ", detected=" << sourceprop.bitsperpixel <<
			" do not match, aborting";
		return false;
	}


	//
	// Processors
	//
	BOOST_FOREACH(const boost::property_tree::ptree::value_type& v, pt.get_child("processor")) {
		DMDFrameProcessor* proc = createProcessor(v.first);
		if (proc) {
			if (proc->configure_from_ptree(pt_general, v.second)) {
				BOOST_LOG_TRIVIAL(info) << "successfully initialized input type " << v.first;
			}
		}
		else {
			BOOST_LOG_TRIVIAL(error) << "don't know processor type " << v.first;
		}
	}

	return true;
}

int main()
{
	string basedir = "../../../";
	read_config(basedir + "democonfig.json");

	return 0;
}
