/*
 * Graph.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <string.h>

#include <FL/fl_ask.H>

#include "Graph.h"
#include "Util.h"
#include "NodeFactory.h"

Graph::Graph(const char * name)
	: Group(name), _file_name(NULL)
{
}

Graph::~Graph()
{
	if (_file_name) free(_file_name);
}

void Graph::file_name(const char * fn)
{
	if (_file_name) free(_file_name);
	_file_name = strdup(fn);
}

int Graph::save_to(const char * fn)
{
	pugi::xml_document doc;
	pugi::xml_node root = doc.append_child("graph");
	if (!Group::save_to(root)) return 0;
	if (!doc.save_file(fn)) {
		fl_alert("Error saving file!");
		return 0;
	}
	file_name(fn);
	return 1;
}

int Graph::load_from(const char * fn)
{
	pugi::xml_document doc;
	pugi::xml_parse_result ret = doc.load_file(fn);
	if (ret.status != pugi::status_ok) {
		fl_alert("Error parsing XML file: %s (err=%d)\n", fn, ret.status);
		return 0;
	}
	pugi::xml_node root = doc.first_child();
	if (0 != strcmp(root.name(), "graph")) {
		fl_alert("Invalid XML file!\n(Root node is not <graph>!)");
		return 0;
	}
	if (!Group::load_from(root)) return 0;
	file_name(fn);
	return 1;
}

