/*
 * GroupNode.h
 *
 *  Created on: Apr 4, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "core/Node.h"
#include "core/Group.h"
#include "core/GroupPorts.h"
#include "core/DynamicNode.h"

class GroupNode: public DynamicNode {
public:
	GroupNode(int x, int y, Group * group = NULL);
	virtual ~GroupNode();

	virtual int save_to(pugi::xml_node & node);
	virtual int load_from(pugi::xml_node & node);

	void inports(GroupPorts * ports);
	void outports(GroupPorts * ports);

	virtual int calc();

private:
	Group * _group;
	GroupPorts * _inports;
	GroupPorts * _outports;
};

