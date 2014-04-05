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
static Array<const char *> * types = NULL;
static Array<new_node_func> * funcs = NULL;

void register_node(const char * group, const char * name, Node* (*new_fn)(int x, int y))
{
	if (!groups || !types || !funcs) {
		groups = new Array<const char *>();
		types = new Array<const char *>();
		funcs = new Array<new_node_func>();
	}
	groups->add(group);
	types->add(name);
	funcs->add(new_fn);
}

Node * new_node(const char * type, int x, int y)
{
	if (!groups || !types || !funcs) return NULL;
	for (int i = 0; i < funcs->count(); i++) {
		if (0 == strcmp(types->get(i), type)) {
			return funcs->get(i)(x, y);
		}
	}
	return NULL;
}

int node_count()
{
	if (!groups || !types || !funcs) return 0;
	return types->count();
}

const char * node_group(int idx)
{
	if (!groups || !types || !funcs) return NULL;
	return groups->get(idx);
}

const char * node_name(int idx)
{
	if (!groups || !types || !funcs) return NULL;
	return types->get(idx);
}

