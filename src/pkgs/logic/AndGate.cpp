/*
 * AndGate.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <pkgs/logic/AndGate.h>

AndGate::AndGate(int x, int y)
	: Node(x, y, "And")
{
	add_input(new Slot(Slot::Bit, "in0"));
	add_input(new Slot(Slot::Bit, "in1"));
	add_output(new Slot(Slot::Bit, "out"));
}

AndGate::~AndGate()
{
}

