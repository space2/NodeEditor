/*
 * SimpleGates.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "core/Node.h"

class NotGate : public Node {
public:
	NotGate(int x, int y);
	virtual ~NotGate() {}
	virtual const char * type() const { return "Not"; }
	virtual int calc();
};

class DualInputLogicGate : public Node {
public:
	DualInputLogicGate(int x, int y);
	virtual ~DualInputLogicGate() {}
	virtual int calc();
protected:
	virtual int calc(int in0, int in1) = 0;
};

class AndGate : public DualInputLogicGate {
public:
	AndGate(int x, int y) : DualInputLogicGate(x, y) {}
	virtual ~AndGate() {}
	virtual const char * type() const { return "And"; }
protected:
	virtual int calc(int in0, int in1);
};

class NandGate : public DualInputLogicGate {
public:
	NandGate(int x, int y) : DualInputLogicGate(x, y) {}
	virtual ~NandGate() {}
	virtual const char * type() const { return "Nand"; }
protected:
	virtual int calc(int in0, int in1);
};

class OrGate : public DualInputLogicGate {
public:
	OrGate(int x, int y) : DualInputLogicGate(x, y) {}
	virtual ~OrGate() {}
	virtual const char * type() const { return "Or"; }
protected:
	virtual int calc(int in0, int in1);
};

class NorGate : public DualInputLogicGate {
public:
	NorGate(int x, int y) : DualInputLogicGate(x, y) {}
	virtual ~NorGate() {}
	virtual const char * type() const { return "Nor"; }
protected:
	virtual int calc(int in0, int in1);
};

class XorGate : public DualInputLogicGate {
public:
	XorGate(int x, int y) : DualInputLogicGate(x, y) {}
	virtual ~XorGate() {}
	virtual const char * type() const { return "Xor"; }
protected:
	virtual int calc(int in0, int in1);
};

