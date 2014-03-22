/*
 * Graph.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <string.h>

#include <FL/fl_ask.H>

#include "Graph.h"
#include "NodeFactory.h"

Graph::Graph(const char * name)
	: _name(strdup(name))
{
}

Graph::~Graph()
{
}

void Graph::clear()
{
	_nodes.clear();
	_conns.clear();
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
	clear();
	pugi::xml_document doc;
	pugi::xml_parse_result ret = doc.load_file(fn);
	if (ret.status != pugi::status_ok) {
		fl_alert("Error parsing XML file: %s (err=%d)\n", fn, ret.status);
		return 0;
	}
	pugi::xml_node root = doc.first_child();
	if (0 != strcmp(root.name(), "graph")) {
		fl_alert("Invalid XML file!\n(Root node is not <graph>!)");
		return 0;
	}
	pugi::xml_node child = root.first_child();
	while (!child.empty()) {
		if (0 == strcmp(child.name(), "node")) {
			const char * name = child.attribute("name").as_string();
			int x = child.attribute("x").as_int();
			int y = child.attribute("y").as_int();
			Node * node = new_node(name, x, y);
			if (!node) {
				fl_alert("Cannot create node of type '%s'!", name);
				return 0;
			} else {
				_nodes.add(node);
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
	return 1;
}
