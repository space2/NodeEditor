/*
 * GroupPorts.h
 *
 *  Created on: Apr 4, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "core/DynamicNode.h"

class GroupPorts: public DynamicNode {
public:
	GroupPorts(int x, int y);
	virtual ~GroupPorts();
	virtual int can_be_removed() const { return 0; };
	virtual const char * type() const { return "Ports"; }
};

