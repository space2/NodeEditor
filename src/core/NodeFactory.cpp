/*
 * NodeFactory.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <stddef.h>

#include "core/NodeFactory.h"
#include "core/StrPtr.h"

#include "Array.h"
typedef Node* (*new_node_func)(int x, int y);

static ArrayO<NodeFactory*> * types = NULL;

void register_node_factory(NodeFactory * nf)
{
	if (!types) {
		types = new ArrayO<NodeFactory *>();
	}
	types->add(nf);
}

NodeFactory * find_node_factory(const char * type)
{
	if (!types) return NULL;
	for (int i = 0; i < types->count(); i++) {
		NodeFactory * nf = types->get(i);
		if (0 == strcmp(nf->type(), type)) {
			return nf;
		}
	}
	return NULL;
}

int unregiser_node_factory(const char * type)
{
	if (!types) return 0;
	for (int i = 0; i < types->count(); i++) {
		if (0 == strcmp(types->get(i)->type(), type)) {
			types->remove_at(i);
			return 1;
		}
	}
	return 0;
}

Node * new_node(const char * type, int x, int y)
{
	if (!types) return 0;
	for (int i = 0; i < types->count(); i++) {
		if (0 == strcmp(types->get(i)->type(), type)) {
			return types->get(i)->create_new(x, y);
		}
	}
	return NULL;
}

int node_factory_count()
{
	if (!types) return 0;
	return types->count();
}

NodeFactory * node_factory(int idx)
{
	if (!types) return NULL;
	return types->get(idx);
}

