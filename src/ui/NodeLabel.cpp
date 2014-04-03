/*
 * NodeLabel.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <FL/Fl.H>

#include "ui/NodeLabel.h"
#include "ui/DnD.h"

NodeLabel::NodeLabel(int x, int y, int w, int h, const char * l)
	: Fl_Box(x, y, w, h, l)
{
	align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
}

NodeLabel::~NodeLabel()
{
}

int NodeLabel::handle(int event)
{
	if (event == FL_PUSH) {
		if (Fl::event_button() == FL_LEFT_MOUSE) {
			// Drag'n'Drop
			char buff[256];
			sprintf(buff, "%s%s", kDndPrefixAdd, label());
			Fl::copy(buff, strlen(buff), 0);
			Fl::dnd();
			return 1;
		}
	}
	return Fl_Box::handle(event);
}
