/*
 * Node.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "Node.h"

static const int kNodeWidth = 100;
static const int kNodeBorder = 5;
static const int kNodeHeader = 10;
static const int kNodeSlotSize = 10;
static const int kSlotAreaWidth = 30;

Node::Node(int x, int y, const char * name)
	: _x(x), _y(y), _name(name)
{
}

Node::~Node()
{
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
