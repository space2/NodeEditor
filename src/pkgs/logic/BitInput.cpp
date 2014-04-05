/*
 * BitInput.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "BitInput.h"
#include "core/NodeFactory.h"

BitInput::BitInput(int x, int y)
	: Node(x, y), _val("value")
{
	add_output(new Slot("out"));
	_val.set_bit(0);
}

BitInput::~BitInput()
{
}

int BitInput::save_to(pugi::xml_node & node)
{
	_val.save_to(node, "val");
	return Node::save_to(node);
}

int BitInput::load_from(pugi::xml_node & node)
{
	_val.load_from(node, "val");
	return Node::load_from(node);
}

int BitInput::calc()
{
	_outputs[0]->set(&_val);
	return _outputs[0]->changed();
}

REGISTER_NODE(Logic,BitInput,BitInput)

