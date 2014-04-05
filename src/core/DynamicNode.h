/*
 * DynamicNode.h
 *
 *  Created on: Apr 4, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "core/Node.h"

class DynamicNode : public Node {
public:
	DynamicNode(int x, int y);
	virtual ~DynamicNode();

	int copy_output(Slot * slot);
	int copy_input(Slot * slot);

	virtual int save_to(pugi::xml_node & node);
	virtual int load_from(pugi::xml_node & node);
};

