/*
 * Graph.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef GRAPH_H_
#define GRAPH_H_

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

	void add(Connection * conn);
	int connection_count() const { return _conns.count(); }
	Connection * connection(int idx) { return _conns[idx]; }
	const Connection * connection(int idx) const { return _conns[idx]; }
private:
	char * _name;
	ArrayO<Node*> _nodes;
	ArrayO<Connection*> _conns;
};

#endif /* GRAPH_H_ */