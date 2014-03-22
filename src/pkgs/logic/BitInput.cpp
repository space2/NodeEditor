/*
 * BitInput.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "BitInput.h"

BitInput::BitInput(int x, int y)
	: Node(x, y, "BitInput"), _val(0)
{
	add_output(new Slot(Slot::Bit, "out"));
}

BitInput::~BitInput()
{
}

int BitInput::save_to(pugi::xml_node & node)
{
	node.append_attribute("val").set_value(_val);
	return Node::save_to(node);
}

int BitInput::load_from(pugi::xml_node & node)
{
	_val = node.attribute("val").as_int(_val);
	return Node::load_from(node);
}
