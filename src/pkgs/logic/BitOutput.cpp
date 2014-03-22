/*
 * BitOutput.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <pkgs/logic/BitOutput.h>

BitOutput::BitOutput(int x, int y)
	: Node(x, y, "BitOutput"), _val(0)
{
	add_input(new Slot(Slot::Bit, "in"));
}

BitOutput::~BitOutput()
{
}

