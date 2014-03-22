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
protected:
	void draw();
private:
	Node * _node;
};

#endif /* NODEUI_H_ */
