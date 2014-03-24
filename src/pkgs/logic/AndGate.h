/*
 * AndGate.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef ANDGATE_H_
#define ANDGATE_H_

#include "core/Node.h"

class AndGate: public Node {
public:
	AndGate(int x, int y);
	virtual ~AndGate();
	virtual int calc();
};

#endif /* ANDGATE_H_ */
