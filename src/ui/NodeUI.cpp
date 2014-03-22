/*
 * NodeUI.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <FL/fl_draw.H>

#include "NodeUI.h"

NodeUI::NodeUI(Node * node)
	: _node(node), _sel(0), _high(0)
{
	update_color();
}

NodeUI::~NodeUI() {
}

int NodeUI::input_count() const
{
	return _node->input_count();
}

int NodeUI::output_count() const
{
	return _node->output_count();
}

const char * NodeUI::input_name(int idx) const
{
	return _node->input(idx)->name();
}

const char * NodeUI::output_name(int idx) const
{
	return _node->output(idx)->name();
}

void NodeUI::draw(int dx, int dy)
{
	int x = _node->x() + dx, y = _node->y() + dy, w = _node->w(), h = _node->h();

	// Render box
	fl_draw_box(FL_UP_BOX, x, y, w, h, _color);

	// Render label
	fl_color(FL_BLACK);
	fl_font(FL_HELVETICA, 10);
	fl_draw(_node->name(), x, y, w, h, FL_ALIGN_TOP | FL_ALIGN_INSIDE, NULL, 0);

	// Draw extra overlays
	fl_color(FL_DARK2);
	fl_xyline(x + 2, y + 13, x + w - 2);
	fl_font(FL_HELVETICA, 8);
	for (int i = 0; i < input_count(); i++) {
		int xx = x;
		int yy = y + 20 + i * 10;
		fl_color(40);
		fl_pie(xx-4, yy-4, 9, 9, -90, 90);
		fl_color(46);
		fl_pie(xx-3, yy-3, 7, 7, -90, 90);
		fl_color(50);
		fl_line(xx+1, yy-3, xx+1, yy+3);
		fl_color(FL_DARK3);
		fl_draw(input_name(i), xx + 5, yy + 1);
	}
	for (int i = 0; i < output_count(); i++) {
		int xx = x + w - 1;
		int yy = y + 20 + i * 10;
		fl_color(40);
		fl_pie(xx-4, yy-4, 9, 9, 90, 270);
		fl_color(46);
		fl_pie(xx-3, yy-3, 7, 7, 90, 270);
		fl_color(40);
		fl_line(xx, yy-3, xx, yy+3);
		fl_color(FL_DARK3);
		int tw = 0, th = 0;
		fl_measure(output_name(i), tw, th, 0);
		fl_draw(output_name(i), xx - 5 - tw, yy + 1);
	}
}

int NodeUI::inside(int mx, int my)
{
	int x = _node->x(), y = _node->y(), w = _node->w(), h = _node->h();
	return mx >= x && my >= y && mx < x + w && my < y + h;
}

int NodeUI::find_input(int x, int y)
{
	return _node->find_input(x, y);
}

int NodeUI::find_output(int x, int y)
{
	return _node->find_output(x, y);
}

void NodeUI::update_color() {
	if (_sel) {
		_color = _high ? 167 : 166;
	} else {
		_color = _high ? 51 : 49;
	}
}

void NodeUI::move(int dx, int dy)
{
	_node->move(dx, dy);
}
