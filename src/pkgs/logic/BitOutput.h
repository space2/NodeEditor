/*
 * BitOutput.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef BITOUTPUT_H_
#define BITOUTPUT_H_

#include "core/Node.h"

class BitOutput: public Node {
public:
	BitOutput(int x, int y);
	virtual ~BitOutput();
private:
	int _val;
};

#endif /* BITOUTPUT_H_ */
