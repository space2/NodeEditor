/*
 * GroupNodeFactory.cpp
 *
 *  Created on: Apr 5, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "ui/GroupNodeFactory.h"

GroupNodeFactory::GroupNodeFactory(Group * grp)
	: NodeFactory(kMacroGroup, grp->name())
{
	pugi::xml_node n = _xml.append_child("node");
	n.append_attribute("type").set_value(grp->type());
	n.append_attribute("x").set_value(grp->x());
	n.append_attribute("y").set_value(grp->y());
	grp->save_to(n);
}

GroupNodeFactory::~GroupNodeFactory()
{
}

Node * GroupNodeFactory::create_new(int x, int y)
{
	pugi::xml_node child = _xml.first_child();
	const char * name = child.attribute("type").as_string();
	Node * clone = new_node(name, x, y);
	clone->load_from(child);
	return clone;
}

