/*
 * AndGate.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "pkgs/logic/AndGate.h"
#include "core/NodeFactory.h"

AndGate::AndGate(int x, int y)
	: Node(x, y, "And")
{
	add_input(new Slot("in0"));
	add_input(new Slot("in1"));
	add_output(new Slot("out"));
}

AndGate::~AndGate()
{
}

int AndGate::calc()
{
	Slot * in0 = _inputs[0];
	Slot * in1 = _inputs[1];
	Slot * out = _outputs[0];
	if (in0->type() != Slot::Bit || in1->type() != Slot::Bit) {
		out->set_undefined();
	} else if (in0->as_bit() == 1 && in1->as_bit() == 1) {
		out->set_bit(1);
	} else {
		out->set_bit(0);
	}
	return out->changed();
}

REGISTER_NODE(Logic,And,AndGate)
