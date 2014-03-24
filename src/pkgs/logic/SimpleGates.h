/*
 * SimpleGates.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef SIMPLEGATES_H_
#define SIMPLEGATES_H_

#include "core/Node.h"

class NotGate : public Node {
public:
	NotGate(int x, int y);
	virtual ~NotGate() {}
	virtual int calc();
};

class DualInputLogicGate : public Node {
public:
	DualInputLogicGate(int x, int y, const char * name);
	virtual ~DualInputLogicGate() {}
	virtual int calc();
protected:
	virtual int calc(int in0, int in1) = 0;
};

class AndGate : public DualInputLogicGate {
public:
	AndGate(int x, int y) : DualInputLogicGate(x, y, "And") {}
	virtual ~AndGate() {}
protected:
	virtual int calc(int in0, int in1);
};

class NandGate : public DualInputLogicGate {
public:
	NandGate(int x, int y) : DualInputLogicGate(x, y, "Nand") {}
	virtual ~NandGate() {}
protected:
	virtual int calc(int in0, int in1);
};

class OrGate : public DualInputLogicGate {
public:
	OrGate(int x, int y) : DualInputLogicGate(x, y, "Or") {}
	virtual ~OrGate() {}
protected:
	virtual int calc(int in0, int in1);
};

class NorGate : public DualInputLogicGate {
public:
	NorGate(int x, int y) : DualInputLogicGate(x, y, "Nor") {}
	virtual ~NorGate() {}
protected:
	virtual int calc(int in0, int in1);
};

class XorGate : public DualInputLogicGate {
public:
	XorGate(int x, int y) : DualInputLogicGate(x, y, "Xor") {}
	virtual ~XorGate() {}
protected:
	virtual int calc(int in0, int in1);
};

#endif /* ANDGATE_H_ */
