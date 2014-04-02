/*
 * NodeUI.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "core/Node.h"

class NodeUI {
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
	int find_input(int x, int y);
	int find_output(int x, int y);
	int is_edit_area(int x, int y);

	int edit();

	void move(int dx, int dy);
	void draw(int dx, int dy);

	Node * node() { return _node; }
	const Node * node() const { return _node; }

private:
	void update_color();

	Node * _node;
	int _color;
	int _sel, _high;
};

