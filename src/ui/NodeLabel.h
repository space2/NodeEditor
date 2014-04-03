/*
 * NodeLabel.h
 *
 *  Created on: Apr 3, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include <FL/Fl_Box.H>

class NodeLabel: public Fl_Box {
public:
	NodeLabel(int x, int y, int w, int h, const char * l = 0);
	virtual ~NodeLabel();
protected:
	int handle(int event);
};

