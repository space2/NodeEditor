/*
 * BitInput.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "core/Node.h"

class BitInput: public Node {
public:
	BitInput(int x, int y);
	virtual ~BitInput();

	virtual int save_to(pugi::xml_node & node);
	virtual int load_from(pugi::xml_node & node);

	virtual const Slot * show_slot() const { return &_val; }
	virtual Slot * edit_slot() { return &_val; }

	virtual int calc();
private:
	Slot _val;
};

