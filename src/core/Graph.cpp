/*
 * Graph.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <string.h>

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

