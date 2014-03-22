/*
 * Workspace.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "Workspace.h"

Workspace::Workspace(int x, int y, int w, int h, const char * l)
	: Fl_Group(x, y, w, h, l)
{
	end();
}

Workspace::~Workspace()
{
}

void Workspace::draw()
{
	Fl_Group::draw();
}

int Workspace::handle(int event)
{
	return Fl_Group::handle(event);
}


