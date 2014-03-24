/*
 * Connection.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "Connection.h"

Connection::Connection(Node * from, int out_idx, Node * to, int in_idx)
	: _from(from), _to(to), _out_idx(out_idx), _in_idx(in_idx)
{
}

Connection::~Connection()
{
}

