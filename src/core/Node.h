/*
 * Node.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef NODE_H_
#define NODE_H_

#include "Array.h"
#include "Slot.h"

class Node {
public:
	Node(int x, int y, const char * name);
	virtual ~Node();

	const char * name() const { return _name; }

	int input_count() const { return _inputs.count(); }
	const Slot * input(int idx) const { return _inputs[idx]; }
	int output_count() const { return _outputs.count(); }
	const Slot * output(int idx) const { return _outputs[idx]; }

	int x() const { return _x; }
	int y() const { return _y; }
	int w() const;
	int h() const;

	int input_x(int idx) const;
	int input_y(int idx) const;
	int output_x(int idx) const;
	int output_y(int idx) const;

protected:
	void add_output(Slot * slot) { _outputs.add(slot); }
	void add_input(Slot * slot) { _inputs.add(slot); }
private:
	int _x, _y;
	const char * _name;
	ArrayO<Slot*> _inputs;
	ArrayO<Slot*> _outputs;
};

#endif /* NODE_H_ */
