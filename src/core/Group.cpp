/*
 * Group.cpp
 *
 *  Created on: Apr 4, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <assert.h>

#include <FL/fl_ask.H>

#include "Group.h"
#include "Graph.h"
#include "Util.h"
#include "NodeFactory.h"

static const int kRangeExtra = 100;

Group::Group(int x, int y)
	: DynamicNode(x, y), _inports(NULL), _outports(NULL)
{
}

Group::~Group()
{
}

void Group::dirty(int v)
{
	Node::dirty(v);
	if (!v) {
		// Clear children
		for (int i = 0; i < _nodes.count(); i++) {
			_nodes[i]->dirty(v);
		}
	}
}

void Group::clear()
{
	_nodes.clear();
	_conns.clear();
}

void Group::add(Node * node)
{
	_nodes.add(node);
	node->parent(this);
	dirty(1);
}

void Group::remove(Node * node, int keep)
{
	if (!keep) {
		for (int i = _conns.count()-1; i >= 0; i--) {
			if (_conns[i]->from() == node || _conns[i]->to() == node) {
				_conns.remove_at(i, keep);
			}
		}
	}
	_nodes.remove(node, keep);
	if (node->parent() == this) node->parent(NULL);
	dirty(1);
}

void Group::add(Connection * conn)
{
	_conns.add(conn);
	dirty(1);
}

void Group::remove(Connection * conn, int keep)
{
	_conns.remove(conn, keep);
	dirty(1);
}

int Group::save_children_to(pugi::xml_node & root)
{
	for (int i = 0; i < _nodes.count(); i++) {
		Node * n = _nodes[i];
		pugi::xml_node node = root.append_child("node");
		node.append_attribute("id").set_value(i);
		node.append_attribute("type").set_value(n->type());
		node.append_attribute("x").set_value(n->x());
		node.append_attribute("y").set_value(n->y());
		n->save_to(node);
	}
	for (int i = 0; i < _conns.count(); i++) {
		Connection * conn = _conns[i];
		pugi::xml_node node = root.append_child("connection");
		node.append_attribute("id").set_value(i);
		node.append_attribute("from").set_value(_nodes.indexof(const_cast<Node*>(conn->from())));
		node.append_attribute("out").set_value(conn->out_idx());
		node.append_attribute("to").set_value(_nodes.indexof(const_cast<Node*>(conn->to())));
		node.append_attribute("in").set_value(conn->in_idx());
	}
	dirty(0);
	return 1;
}

int Group::load_children_from(pugi::xml_node & root)
{
	clear();
	pugi::xml_node child = root.first_child();
	while (!child.empty()) {
		if (0 == strcmp(child.name(), "node")) {
			const char * type = child.attribute("type").as_string();
			int x = child.attribute("x").as_int();
			int y = child.attribute("y").as_int();
			Node * node = new_node(type, x, y);
			if (!node) {
				fl_alert("Cannot create node of type '%s'!", type);
				return 0;
			} else {
				_nodes.add(node);
				node->parent(this);
				node->load_from(child);
			}
		} else if (0 == strcmp(child.name(), "connection")) {
			int from = child.attribute("from").as_int();
			int out_idx = child.attribute("out").as_int();
			int to = child.attribute("to").as_int();
			int in_idx = child.attribute("in").as_int();
			Connection * conn = new Connection(_nodes[from], out_idx, _nodes[to], in_idx);
			_conns.add(conn);
		} else {
			fl_alert("Invalid xml node: '%s'!", child.name());
			return 0;
		}
		child = child.next_sibling();
	}
	dirty(0);
	return 1;
}

void Group::calc_range(int & min_x, int & min_y, int & max_x, int & max_y)
{
	min_x = min_y = max_x = max_y = 0;
	for (int i = 0; i < _nodes.count(); i++) {
		Node * node = _nodes[i];
		min_x = Util::min(min_x, node->x());
		min_y = Util::min(min_y, node->y());
		max_x = Util::max(max_x, node->x() + node->w());
		max_y = Util::max(max_y, node->y() + node->h());
	}
	min_x -= kRangeExtra;
	min_y -= kRangeExtra;
	max_x += kRangeExtra;
	max_y += kRangeExtra;
}


void Group::calc_children()
{
	for (int i = 0; i < _nodes.count(); i++) {
		_nodes[i]->pre_calc();
	}

	// Send the current values through all the connections (in case the connections changed, but
	// not the nodes
	// Find all connections coming from this slot and transmit the new value
	for (int c = 0; c < _conns.count(); c++) {
		Connection * conn = _conns[c];
		Slot * begin = conn->from()->output(conn->out_idx());
		Slot * end = conn->to()->input(conn->in_idx());
		end->set(begin);
	}

	// Simply loop until there is no more change in the graph
	for (int tries = 0; tries < 1000; tries++) {
		int changed = 0; // Check if anything changed in this iteration
		for (int i = 0; i < _nodes.count(); i++) {
			Node * node = _nodes[i];
			if (node->calc()) {
				// This node did something, let's find the changed output and transmit it
				for (int oidx = 0; oidx < node->output_count(); oidx++) {
					Slot * slot = node->output(oidx);
					if (slot->changed()) {
						// Find all connections coming from this slot and transmit the new value
						for (int c = 0; c < _conns.count(); c++) {
							Connection * conn = _conns[c];
							if (conn->from() == node && conn->out_idx() == oidx) {
								// Got a matching connection, transmit the value
								Slot * end = conn->to()->input(conn->in_idx());
								end->set(slot);
								changed++;
							}
						}
						slot->changed(0);
					}
				}
			}
		}
		printf("iter#%d changed=%d\n", tries, changed);
		if (!changed) break;
	}

	for (int i = 0; i < _nodes.count(); i++) {
		_nodes[i]->post_calc();
	}
}

int Group::save_to(pugi::xml_node & xml)
{
	if (!DynamicNode::save_to(xml)) return 0;
	save_children_to(xml);
	for (int i = 0; i < node_count(); i++) {
		Node * n = node(i);
		if (n == _inports) {
			xml.append_attribute("inports").set_value(i);
		}
		if (n == _outports) {
			xml.append_attribute("outports").set_value(i);
		}
	}
	return 1;
}

int Group::load_from(pugi::xml_node & xml)
{
	if (!DynamicNode::load_from(xml)) return 0;
	load_children_from(xml);
	_inports = dynamic_cast<GroupPorts*>(node(xml.attribute("inports").as_int()));
	_outports = dynamic_cast<GroupPorts*>(node(xml.attribute("outports").as_int()));
	return 1;
}

void Group::add_inports(GroupPorts * ports)
{
	assert(_inports == NULL);
	_inports = ports;
	add(ports);
}

void Group::add_outports(GroupPorts * ports)
{
	assert(_outports == NULL);
	_outports = ports;
	add(ports);
}

int Group::calc()
{
	int ret = 0;

	// Copy input values from GroupNode to internal ports
	if (_inports) {
		for (int i = 0; i < input_count(); i++) {
			_inports->output(i)->set(input(i));
			if (_inports->output(i)->changed()) ret = 1;
		}
	}

	// Calculate the internal network
	calc_children();

	// Copy back outputs
	if (_outports) {
		for (int i = 0; i < output_count(); i++) {
			output(i)->set(_outports->input(i));
			if (output(i)->changed()) ret = 1;
		}
	}

	return 1;
}

int Group::swap_slots(int output, int idx0, int idx1)
{
	if (!Node::swap_slots(output, idx0, idx1)) return 0;
	if (output && _outports) {
		_outports->swap_slots(!output, idx0, idx1);
		swap_slots(_outports, !output, idx0, idx1);
	}
	if (!output && _inports) {
		_inports->swap_slots(!output, idx0, idx1);
		swap_slots(_inports, !output, idx0, idx1);
	}
	return 1;
}

int Group::swap_slots(Node * node, int output, int idx0, int idx1)
{
	for (int i = 0; i < _conns.count(); i++) {
		Connection * conn = _conns[i];
		if (output && conn->from() == node) {
			if (conn->out_idx() == idx0) {
				conn->from(node, idx1);
			} else if (conn->out_idx() == idx1) {
				conn->from(node, idx0);
			}
		} else if (!output && conn->to() == node) {
			if (conn->in_idx() == idx0) {
				conn->to(node, idx1);
			} else if (conn->in_idx() == idx1) {
				conn->to(node, idx0);
			}
		}
	}
	return 1;
}

REGISTER_NODE(_,Group,Group);
