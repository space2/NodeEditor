/*
 * NodeUI.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef NODEUI_H_
#define NODEUI_H_

#include <FL/Fl_Box.H>

class NodeUI: public Fl_Box {
public:
	NodeUI(int x, int y, int w, int h, const char * l = 0);
	virtual ~NodeUI();

	int input_count() const;
	const char * input_name(int idx) const;
	int output_count() const;
	const char * output_name(int idx) const;
protected:
	void draw();
};

#endif /* NODEUI_H_ */
