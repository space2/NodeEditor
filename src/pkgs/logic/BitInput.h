/*
 * BitInput.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef BITINPUT_H_
#define BITINPUT_H_

#include "core/Node.h"

class BitInput: public Node {
public:
	BitInput(int x, int y);
	virtual ~BitInput();

	int value() const { return _val; }
	void value(int val) { _val = val; }

	virtual int save_to(pugi::xml_node & node);
	virtual int load_from(pugi::xml_node & node);
private:
	int _val;
};

#endif /* BITINPUT_H_ */
