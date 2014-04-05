/*
 * NodeFactory.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "Node.h"

extern void register_node(const char * group, const char * type, Node* (*new_fn)(int x, int y));
extern int node_count();
extern const char * node_group(int idx);
extern const char * node_name(int idx);
extern Node * new_node(const char * type, int x, int y);

#define REGISTER_NODE(grp,type,cls) \
static Node* __new__##grp##__##type(int x, int y) { \
	return new cls(x, y); \
} \
__attribute__((constructor)) \
void __init__##grp##__##type() { \
	register_node(#grp, #type, __new__##grp##__##type); \
}

