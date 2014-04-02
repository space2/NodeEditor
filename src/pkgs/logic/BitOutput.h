/*
 * BitOutput.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "core/Node.h"

class BitOutput: public Node {
public:
	BitOutput(int x, int y);
	virtual ~BitOutput();
	virtual const Slot * show_slot() const { return _inputs[0]; }
};

