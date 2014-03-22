/*
 * NodeFactory.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef NODEFACTORY_H_
#define NODEFACTORY_H_

#include "Node.h"

extern void register_node(const char * group, const char * name, Node* (*new_fn)(int x, int y));
extern Node * new_node(const char * name, int x, int y);

#define REGISTER_NODE(grp,name,cls) \
static Node* __new__##grp##__##name(int x, int y) { \
	return new cls(x, y); \
} \
__attribute__((constructor)) \
void __init__##grp##__##name() { \
	register_node(#grp, #name, __new__##grp##__##name); \
}

#endif /* NODEFACTORY_H_ */
