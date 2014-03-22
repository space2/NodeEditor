/*
 * NodeUI.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef NODEUI_H_
#define NODEUI_H_

#include <FL/Fl_Box.H>

#include "core/Node.h"

class NodeUI: public Fl_Box {
public:
	NodeUI(Node * node);
	virtual ~NodeUI();

	int input_count() const;
	const char * input_name(int idx) const;
	int output_count() const;
	const char * output_name(int idx) const;

	int selected() const { return _sel; }
	void selected(int v) { _sel = v; update_color(); }
	int highlighted() const { return _high; }
	void highlighted(int v) { _high = v; update_color(); }

	int inside(int mx, int my);
protected:
	void draw();
private:
	void update_color();

	Node * _node;
	int _sel, _high;
};

#endif /* NODEUI_H_ */
