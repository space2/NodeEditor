/*
 * GroupPorts.cpp
 *
 *  Created on: Apr 4, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "core/GroupPorts.h"
#include "core/NodeFactory.h"

GroupPorts::GroupPorts(int x, int y)
	: DynamicNode(x, y, "Ports")
{
}

GroupPorts::~GroupPorts()
{
}

REGISTER_NODE(_,Ports,GroupPorts);
