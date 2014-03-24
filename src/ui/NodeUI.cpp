/*
 * NodeUI.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <FL/fl_draw.H>

#include "core/Util.h"

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
		int yy = dy + _node->input_y(i);
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
		int yy = dy + _node->output_y(i);
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

	// Draw value
	const Slot * slot = _node->edit_slot();
	if (!slot) slot = _node->show_slot();
	if (slot) {
		if (slot->type() == Slot::Bit) {
			const char * v = Util::bit2string(slot->as_bit());
			fl_color(FL_BLACK);
			fl_font(FL_COURIER, 16);
			fl_draw(v, x, y + 15, w, h - 15, FL_ALIGN_CENTER, NULL, 0);
		} else {
			// TODO: Render the other types
		}
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

int NodeUI::is_edit_area(int x, int y)
{
	Slot * slot = _node->edit_slot();
	if (!slot) return 0;
	int xx, yy, ww, hh;
	_node->get_client_rect(xx, yy, ww, hh);
	return (x >= xx) && (y >= yy) && (x < xx + ww) && (y < yy + hh);
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

int NodeUI::edit()
{
	Slot * slot = _node->edit_slot();
	if (slot->type() == Slot::Bit) {
		// Toggle bit
		slot->set_bit(!slot->as_bit());
		return 1;
	}
	return 0;
}
