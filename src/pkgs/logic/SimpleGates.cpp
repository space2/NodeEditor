/*
 * SimpleGates.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "pkgs/logic/SimpleGates.h"
#include "core/NodeFactory.h"

NotGate::NotGate(int x, int y)
	: Node(x, y, "NotGate")
{
	add_input(new Slot("in"));
	add_output(new Slot("out"));
}

int NotGate::calc()
{
	Slot * in = _inputs[0];
	Slot * out = _outputs[0];
	if (in->type() == Slot::Bit) {
		out->set_bit(!in->as_bit());
	} else {
		out->set_undefined();
	}
	return out->changed();
}

DualInputLogicGate::DualInputLogicGate(int x, int y, const char * name)
	: Node(x, y, name)
{
	add_input(new Slot("in0"));
	add_input(new Slot("in1"));
	add_output(new Slot("out"));
}

int DualInputLogicGate::calc()
{
	int inv0 = -1, inv1 = -1;
	Slot * in0 = _inputs[0];
	Slot * in1 = _inputs[1];
	Slot * out = _outputs[0];
	if (in0->type() == Slot::Bit) {
		inv0 = in0->as_bit();
	}
	if (in1->type() == Slot::Bit) {
		inv1 = in1->as_bit();
	}
	int outv = calc(inv0, inv1);
	if (outv < 0) {
		out->set_undefined();
	} else {
		out->set_bit(outv);
	}
	return out->changed();
}

int AndGate::calc(int inv0, int inv1)
{
	if (inv0 == 1 && inv1 == 1) return 1;
	if (inv0 == 0 || inv1 == 0) return 0;
	return -1;
}

int NandGate::calc(int inv0, int inv1)
{
	if (inv0 == 1 && inv1 == 1) return 0;
	if (inv0 == 0 || inv1 == 0) return 1;
	return -1;
}

int OrGate::calc(int inv0, int inv1)
{
	if (inv0 == 1 || inv1 == 1) return 1;
	if (inv0 == 0 && inv1 == 0) return 0;
	return -1;
}

int NorGate::calc(int inv0, int inv1)
{
	if (inv0 == 1 || inv1 == 1) return 0;
	if (inv0 == 0 && inv1 == 0) return 1;
	return -1;
}

int XorGate::calc(int inv0, int inv1)
{
	if (inv0 == -1 || inv1 == -1) return -1;
	return inv0 ^ inv1;
}

REGISTER_NODE(Logic,Not,NotGate)
REGISTER_NODE(Logic,And,AndGate)
REGISTER_NODE(Logic,Nand,NandGate)
REGISTER_NODE(Logic,Or,OrGate)
REGISTER_NODE(Logic,Nor,NorGate)
REGISTER_NODE(Logic,Xor,XorGate)
