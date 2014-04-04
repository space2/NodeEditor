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

class Group {
public:
	Group(const char * name);
	virtual ~Group();

	int dirty() const { return _dirty; }
	void dirty(int v) { _dirty = v; }

	void add(Node * node);
	int node_count() const { return _nodes.count(); }
	Node * node(int idx) { return _nodes[idx]; }
	const Node * node(int idx) const { return _nodes[idx]; }
	void remove(Node * node, int keep = 0);

	void add(Connection * conn);
	int connection_count() const { return _conns.count(); }
	Connection * connection(int idx) { return _conns[idx]; }
	const Connection * connection(int idx) const { return _conns[idx]; }
	void remove(Connection * conn, int keep = 0) { _conns.remove(conn, keep); _dirty = 1; }

	int save_to(pugi::xml_node & root);
	int load_from(pugi::xml_node & root);

	void calc_range(int & min_x, int & min_y, int & max_x, int & max_y);

	void calc();
private:
	void clear();

	char * _name;
	ArrayO<Node*> _nodes;
	ArrayO<Connection*> _conns;
	int _dirty;
};

