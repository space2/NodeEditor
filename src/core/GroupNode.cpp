/*
 * GroupNode.cpp
 *
 *  Created on: Apr 4, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "core/GroupNode.h"
#include "core/NodeFactory.h"

GroupNode::GroupNode(int x, int y, Group * group)
	: DynamicNode(x, y, "Group"), _group(group), _inports(NULL), _outports(NULL)
{
	if (_group == NULL) {
		_group = new Group("");
	}
}

GroupNode::~GroupNode()
{
	delete _group;
}

int GroupNode::save_to(pugi::xml_node & node)
{
	if (!DynamicNode::save_to(node)) return 0;
	_group->save_to(node);
	for (int i = 0; i < _group->node_count(); i++) {
		Node * n = _group->node(i);
		if (n == _inports) {
			node.append_attribute("inports").set_value(i);
		}
		if (n == _outports) {
			node.append_attribute("outports").set_value(i);
		}
	}
	return 1;
}

int GroupNode::load_from(pugi::xml_node & node)
{
	if (!DynamicNode::load_from(node)) return 0;
	_group->load_from(node);
	_inports = dynamic_cast<GroupPorts*>(_group->node(node.attribute("inports").as_int()));
	_outports = dynamic_cast<GroupPorts*>(_group->node(node.attribute("outports").as_int()));
	return 1;
}

void GroupNode::inports(GroupPorts * ports)
{
	_inports = ports;
	_group->add(ports);
}

void GroupNode::outports(GroupPorts * ports)
{
	_outports = ports;
	_group->add(ports);
}

int GroupNode::calc()
{
	int ret = 0;

	// Copy input values from GroupNode to internal ports
	for (int i = 0; i < input_count(); i++) {
		_inports->output(i)->set(input(i));
		if (_inports->output(i)->changed()) ret = 1;
	}

	// Calculate the internal network
	_group->calc();

	// Copy back outputs
	for (int i = 0; i < output_count(); i++) {
		output(i)->set(_outports->input(i));
		if (output(i)->changed()) ret = 1;
	}

	return 1;
}

REGISTER_NODE(_,Group,GroupNode);
