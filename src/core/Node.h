/*
 * Node.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "pugixml/pugixml.hpp"

#include "Array.h"
#include "Slot.h"

class Node {
public:
	Node(int x, int y, const char * name);
	virtual ~Node();

	const char * name() const { return _name; }

	int input_count() const { return _inputs.count(); }
	Slot * input(int idx) { return _inputs[idx]; }
	int output_count() const { return _outputs.count(); }
	Slot * output(int idx) { return _outputs[idx]; }

	virtual const Slot * show_slot() const { return NULL; }
	virtual Slot * edit_slot() { return NULL; }

	virtual void pre_calc() { /* NOP */ }
	virtual int calc() { return 0; /* NOP */ }
	virtual void post_calc() { /* NOP */ }

	int x() const { return _x; }
	int y() const { return _y; }
	int w() const;
	int h() const;

	int input_x(int idx) const;
	int input_y(int idx) const;
	int output_x(int idx) const;
	int output_y(int idx) const;

	int find_input(int x, int y);
	int find_output(int x, int y);

	void get_client_rect(int & x, int & y, int & w, int & h);

	void move(int dx, int dy);

	virtual int save_to(pugi::xml_node & node);
	virtual int load_from(pugi::xml_node & node);

protected:
	void add_output(Slot * slot) { _outputs.add(slot); }
	void add_input(Slot * slot) { _inputs.add(slot); }

	ArrayO<Slot*> _inputs;
	ArrayO<Slot*> _outputs;

private:
	int _x, _y;
	const char * _name;
};

