/*
 * DynamicNode.cpp
 *
 *  Created on: Apr 4, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "core/DynamicNode.h"

DynamicNode::DynamicNode(int x, int y)
	: Node(x, y)
{
}

DynamicNode::~DynamicNode()
{
}

int DynamicNode::copy_output(Slot * slot)
{
	return _outputs.add(new Slot(slot->name()));
}

int DynamicNode::copy_input(Slot * slot)
{
	return _inputs.add(new Slot(slot->name()));
}

int DynamicNode::save_to(pugi::xml_node & node)
{
	if (!Node::save_to(node)) return 0;
	node.append_attribute("nr_inputs").set_value(_inputs.count());
	node.append_attribute("nr_outputs").set_value(_outputs.count());
	return 1;
}

int DynamicNode::load_from(pugi::xml_node & node)
{
	int nr_inputs = node.attribute("nr_inputs").as_int();
	int nr_outputs = node.attribute("nr_outputs").as_int();
	char buff[32];
	for (int i = 0; i < nr_inputs; i++) {
		sprintf(buff, "input%d", i);
		_inputs.add(new Slot(buff));
	}
	for (int i = 0; i < nr_outputs; i++) {
		sprintf(buff, "output%d", i);
		_outputs.add(new Slot(buff));
	}
	// We can load the node properties after the ports are created!
	if (!Node::load_from(node)) return 0;
	return 1;
}
