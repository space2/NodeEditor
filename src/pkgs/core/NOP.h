/*
 * NOP.h
 *
 *  Created on: Apr 2, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "core/Node.h"

class NOP: public Node {
public:
	NOP(int x, int y);
	virtual ~NOP();

	virtual int calc();
};

