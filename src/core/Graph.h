/*
 * Graph.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "Node.h"
#include "Connection.h"
#include "Array.h"

class Graph {
public:
	Graph(const char * name);
	virtual ~Graph();

	void add(Node * node);
	int node_count() const { return _nodes.count(); }
	Node * node(int idx) { return _nodes[idx]; }
	const Node * node(int idx) const { return _nodes[idx]; }
	void remove(Node * node);

	void add(Connection * conn);
	int connection_count() const { return _conns.count(); }
	Connection * connection(int idx) { return _conns[idx]; }
	const Connection * connection(int idx) const { return _conns[idx]; }
	void remove(Connection * conn) { _conns.remove(conn); _dirty = 1; }

	int save_to(const char * fn);
	int load_from(const char * fn);

	int dirty() const { return _dirty; }
	void dirty(int v) { _dirty = v; }

	const char * file_name() const { return _file_name; }

	void calc_range(int & min_x, int & min_y, int & max_x, int & max_y);

	void calc();
private:
	void clear();
	void file_name(const char * fn);

	char * _name;
	char * _file_name;
	ArrayO<Node*> _nodes;
	ArrayO<Connection*> _conns;
	int _dirty;
};

