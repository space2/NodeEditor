/*
 * NodeFactory.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "Node.h"

static const char * kMacroGroup = "Macros";

class NodeFactory {
public:
	NodeFactory(const char * group, const char * type) : _group(group), _type(type) {}
	virtual ~NodeFactory() {}
	const char * group() const { return _group; }
	const char * type() const { return _type; }
	int builtin() const { return strcmp(_group, kMacroGroup); }
	virtual Node * create_new(int x, int y) = 0;
private:
	StrPtr _group;
	StrPtr _type;
};

extern NodeFactory * find_node_factory(const char * type);
extern int unregiser_node_factory(const char * type);
extern void register_node_factory(NodeFactory * nf);
extern int node_factory_count();
extern NodeFactory * node_factory(int idx);
extern Node * new_node(const char * type, int x, int y);

#define REGISTER_NODE(grp,type,cls) \
class __new__##grp##__##type : public NodeFactory { \
public:\
	__new__##grp##__##type() : NodeFactory(#grp, #type) {} \
	virtual Node * create_new(int x, int y) { return new cls(x, y); } \
}; \
__attribute__((constructor)) \
void __init__##grp##__##type() { \
	register_node_factory(new __new__##grp##__##type()); \
}

