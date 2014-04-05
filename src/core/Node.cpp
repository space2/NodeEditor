/*
 * Node.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "Node.h"
#include "Group.h"
#include "NodeFactory.h"

static const int kNodeWidth = 100;
static const int kNodeBorder = 5;
static const int kNodeHeader = 10;
static const int kNodeSlotSize = 10;
static const int kSlotAreaWidth = 30;

Node::Node(int x, int y)
	: _x(x), _y(y), _name(NULL), _sel(0), _dirty(0), _parent(NULL)
{
}

Node::~Node()
{
}

Node * Node::clone()
{
	Node * ret = new_node(type(), x() + 10, y() + 10);
	ret->name(_name);
	return ret;
}

void Node::dirty(int v) {
	_dirty = v;
	if (v) {
		// Notify the parent
		if (_parent) _parent->dirty(v);
	}
}

int Node::w() const
{
	return kNodeWidth;
}

int Node::h() const
{
	int slots = input_count() < output_count() ? output_count() : input_count();
	return 2*kNodeBorder + kNodeHeader + kNodeSlotSize * slots;
}

int Node::input_x(int idx) const
{
	return _x;
}

int Node::input_y(int idx) const
{
	return _y + kNodeBorder + kNodeHeader + kNodeSlotSize * idx + kNodeSlotSize/2;
}

int Node::output_x(int idx) const
{
	return _x + kNodeWidth-1;
}

int Node::output_y(int idx) const
{
	return _y + kNodeBorder + kNodeHeader + kNodeSlotSize * idx + kNodeSlotSize/2;
}

void Node::move(int dx, int dy)
{
	_x += dx;
	_y += dy;
}

int Node::find_input(int x, int y)
{
	x -= _x;
	y -= _y + kNodeBorder + kNodeHeader;
	if (x < 0 || x > kSlotAreaWidth || y < 0) return -1;
	int ret = y / kNodeSlotSize;
	if (ret < _inputs.count()) return ret;
	return -1;
}

int Node::find_output(int x, int y)
{
	x -= _x + kNodeWidth - kSlotAreaWidth;
	y -= _y + kNodeBorder + kNodeHeader;
	if (x < 0 || x > kSlotAreaWidth || y < 0) return -1;
	int ret = y / kNodeSlotSize;
	if (ret < _outputs.count()) return ret;
	return -1;
}

int Node::inside(int mx, int my)
{
	return mx >= x() && my >= y() && mx < x() + w() && my < y() + h();
}

int Node::is_edit_area(int x, int y)
{
	Slot * slot = edit_slot();
	if (!slot) return 0;
	int xx, yy, ww, hh;
	get_client_rect(xx, yy, ww, hh);
	return (x >= xx) && (y >= yy) && (x < xx + ww) && (y < yy + hh);
}

int Node::save_to(pugi::xml_node & node)
{
	// Save node name
	if (!_name.empty()) node.append_attribute("name").set_value(_name);

	// Save input/output names, since they might have changes
	char buff[32];
	for (int i = 0; i < input_count(); i++) {
		sprintf(buff, "input_%d", i);
		node.append_attribute(buff).set_value(input(i)->name());
	}
	for (int i = 0; i < output_count(); i++) {
		sprintf(buff, "output_%d", i);
		node.append_attribute(buff).set_value(output(i)->name());
	}
	return 1;
}

int Node::load_from(pugi::xml_node & node)
{
	// Load node name
	const char * name = node.attribute("name").as_string();
	if (name && name[0]) _name = name;

	// Load input/output names, since they might have changes
	char buff[32];
	for (int i = 0; i < input_count(); i++) {
		sprintf(buff, "input_%d", i);
		pugi::xml_attribute new_name = node.attribute(buff);
		if (!new_name.empty()) input(i)->name(new_name.as_string());
	}
	for (int i = 0; i < output_count(); i++) {
		sprintf(buff, "output_%d", i);
		pugi::xml_attribute new_name = node.attribute(buff);
		if (!new_name.empty()) output(i)->name(new_name.as_string());
	}
	return 1;
}

void Node::get_client_rect(int & x, int & y, int & w, int & h)
{
	x = _x + kSlotAreaWidth;
	y = _y + kNodeHeader;
	w = this->w() - 2 * kSlotAreaWidth;
	h = this->h() - kNodeHeader;
}

int Node::edit()
{
	Slot * slot = edit_slot();
	if (slot->type() == Slot::Bit) {
		// Toggle bit
		slot->set_bit(!slot->as_bit());
		dirty(1);
		return 1;
	}
	return 0;
}
