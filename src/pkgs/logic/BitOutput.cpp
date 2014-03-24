/*
 * BitOutput.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <pkgs/logic/BitOutput.h>
#include "core/NodeFactory.h"

BitOutput::BitOutput(int x, int y)
	: Node(x, y, "BitOutput")
{
	add_input(new Slot("in"));
}

BitOutput::~BitOutput()
{
}

REGISTER_NODE(Logic,BitOutput,BitOutput)
