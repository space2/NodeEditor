/*
 * Group.h
 *
 *  Created on: Apr 4, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "Node.h"
#include "Connection.h"
#include "Array.h"
#include "GroupPorts.h"
#include "DynamicNode.h"

class Group : public DynamicNode {
public:
	Group(int x, int y);
	virtual ~Group();

	virtual const char * type() const { return "Group"; }

	virtual void dirty(int v);

	void add(Node * node);
	int node_count() const { return _nodes.count(); }
	Node * node(int idx) { return _nodes[idx]; }
	const Node * node(int idx) const { return _nodes[idx]; }
	void remove(Node * node, int keep = 0);

	void add(Connection * conn);
	int connection_count() const { return _conns.count(); }
	Connection * connection(int idx) { return _conns[idx]; }
	const Connection * connection(int idx) const { return _conns[idx]; }
	void remove(Connection * conn, int keep = 0);

	virtual int save_to(pugi::xml_node & node);
	virtual int load_from(pugi::xml_node & node);

	void calc_range(int & min_x, int & min_y, int & max_x, int & max_y);

	void add_inports(GroupPorts * ports);
	void add_outports(GroupPorts * ports);

	virtual int can_reorder_slots() const { return 1; }
	virtual int swap_slots(int output, int idx0, int idx1);
	int swap_slots(Node * node, int output, int idx0, int idx1);

	virtual int calc();

private:
	void clear();
	void calc_children();
	int save_children_to(pugi::xml_node & node);
	int load_children_from(pugi::xml_node & node);

	ArrayO<Node*> _nodes;
	ArrayO<Connection*> _conns;
	GroupPorts * _inports;
	GroupPorts * _outports;
};

