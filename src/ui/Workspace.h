/*
 * Workspace.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef WORKSPACE_H_
#define WORKSPACE_H_

#include <FL/Fl_Group.H>

class Workspace : public Fl_Group {
public:
	Workspace(int x, int y, int w, int h, const char * l = 0);
	virtual ~Workspace();
protected:
	void draw();
	int handle(int event);
};

#endif /* WORKSPACE_H_ */
