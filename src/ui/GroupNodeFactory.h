/*
 * GroupNodeFactory.h
 *
 *  Created on: Apr 5, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "pugixml/pugixml.hpp"

#include "core/NodeFactory.h"
#include "core/Group.h"

class GroupNodeFactory : public NodeFactory {
public:
	GroupNodeFactory(Group * grp);
	virtual ~GroupNodeFactory();
	virtual Node * create_new(int x, int y);

	int export_to_file();

	static void scan_exported_files();
private:
	GroupNodeFactory(const char * fn);

	pugi::xml_document _xml;
};

