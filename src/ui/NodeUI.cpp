/*
 * NodeUI.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <FL/fl_draw.H>

#include "NodeUI.h"

static const char * kInputNames[] = {
		"color",
		"mask",
};

static const char * kOutputNames[] = {
		"red",
		"green",
		"blue",
		"alpha",
};

NodeUI::NodeUI(int x, int y, int w, int h, const char * l)
	: Fl_Box(x, y, w, h, l)
{
	box(FL_UP_BOX);
	labelsize(10);
	align(FL_ALIGN_TOP | FL_ALIGN_INSIDE);
}

NodeUI::~NodeUI() {
}

int NodeUI::input_count() const
{
	// FIXME: hardcoded
	return 2;
}

int NodeUI::output_count() const
{
	// FIXME: hardcoded
	return 4;
}

const char * NodeUI::input_name(int idx) const
{
	// FIXME: hardcoded
	return kInputNames[idx];
}

const char * NodeUI::output_name(int idx) const
{
	// FIXME: hardcoded
	return kOutputNames[idx];
}


void NodeUI::draw()
{
	// Render box, label, etc
	Fl_Box::draw();

	// Draw extra overlays
	fl_color(FL_DARK2);
	fl_xyline(x() + 2, y() + 13, x() + w() - 2);
	fl_font(FL_HELVETICA, 8);
	for (int i = 0; i < input_count(); i++) {
		int xx = x();
		int yy = y() + 20 + i * 10;
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
		int xx = x() + w() - 1;
		int yy = y() + 20 + i * 10;
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


