/*
 * NodeFactory.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "core/NodeFactory.h"

#include "Array.h"
typedef Node* (*new_node_func)(int x, int y);

static Array<const char *> * groups = NULL;
static Array<const char *> * names = NULL;
static Array<new_node_func> * funcs = NULL;

void register_node(const char * group, const char * name, Node* (*new_fn)(int x, int y))
{
	if (!groups || !names || !funcs) {
		groups = new Array<const char *>();
		names = new Array<const char *>();
		funcs = new Array<new_node_func>();
	}
	groups->add(group);
	names->add(name);
	funcs->add(new_fn);
}

Node * new_node(const char * name, int x, int y)
{
	if (!groups || !names || !funcs) return NULL;
	for (int i = 0; i < funcs->count(); i++) {
		if (0 == strcmp(names->get(i), name)) {
			return funcs->get(i)(x, y);
		}
	}
	return NULL;
}
