/*
 * Graph.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <string.h>

#include <FL/fl_ask.H>

#include "Graph.h"

Graph::Graph(const char * name)
	: _name(strdup(name))
{
}

Graph::~Graph()
{
}

void Graph::add(Node * node)
{
	_nodes.add(node);
}

void Graph::add(Connection * conn)
{
	_conns.add(conn);
}

int Graph::save_to(const char * fn)
{
	pugi::xml_document doc;
	pugi::xml_node root = doc.append_child("graph");
	for (int i = 0; i < _nodes.count(); i++) {
		Node * n = _nodes[i];
		pugi::xml_node node = root.append_child("node");
		node.append_attribute("id").set_value(i);
		node.append_attribute("name").set_value(n->name());
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
	if (!doc.save_file(fn)) {
		fl_alert("Error saving file!");
		return 0;
	}
	return 1;
}

int Graph::load_from(const char * fn)
{
	// FIXME
	return 0;
}
