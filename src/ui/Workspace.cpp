/*
 * Workspace.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <FL/fl_draw.H>

#include "Workspace.h"
#include "NodeUI.h"

static const int kGridSize = 10;
static const int kGridMajor = 10;
static const int kGridMajorSize = 5;

Workspace::Workspace(int x, int y, int w, int h, const char * l)
	: Fl_Group(x, y, w, h, l)
{
	// FIXME: testing
	new NodeUI(x + 100, y + 100, 100, 60, "Foobar");
	// ---
	end();
}

Workspace::~Workspace()
{
}

void Workspace::draw()
{
	draw_background();
	draw_children();
}

int Workspace::handle(int event)
{
	return Fl_Group::handle(event);
}

void Workspace::draw_background()
{
	fl_rectf(x(), y(), w(), h(), FL_BLACK);
	fl_color(36);
	for (int xx = x(); xx < x() + w(); xx += kGridSize) {
		fl_line(xx, y(), xx, y() + h());
	}
	for (int yy = y(); yy < y() + h(); yy += kGridSize) {
		fl_line(x(), yy, x() + w(), yy);
	}
	fl_color(40);
	for (int xx = x(); xx < x() + w(); xx += kGridSize * kGridMajor) {
		for (int yy = y(); yy < y() + h(); yy += kGridSize * kGridMajor) {
			fl_line(xx - kGridMajorSize, yy, xx + kGridMajorSize, yy);
			fl_line(xx, yy - kGridMajorSize, xx, yy + kGridMajorSize);
		}
	}
}

