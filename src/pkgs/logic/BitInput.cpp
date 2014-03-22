/*
 * BitInput.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "BitInput.h"

BitInput::BitInput(int x, int y)
	: Node(x, y, "BitInput"), _val(0)
{
	add_output(new Slot(Slot::Bit, "out"));
}

BitInput::~BitInput()
{
}

