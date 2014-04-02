/*
 * NOP.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "pkgs/core/NOP.h"
#include "core/NodeFactory.h"

NOP::NOP(int x, int y)
	: Node(x, y, "NOP")
{
	add_input(new Slot("in"));
	add_output(new Slot("out"));
}

NOP::~NOP()
{
}

int NOP::calc()
{
	_outputs[0]->set(_inputs[0]);
	return _outputs[0]->changed();
}

REGISTER_NODE(Core,NOP,NOP)

