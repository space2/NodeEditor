/*
 * Connection.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "Node.h"

class Connection {
public:
	Connection(Node * from, int out_idx, Node * to, int in_idx);
	virtual ~Connection();

	const Node * from() const { return _from; }
	const int out_idx() const { return _out_idx; }
	const Node * to() const { return _to; }
	const int in_idx() const { return _in_idx; }

	void from(Node * n, int idx) { _from = n; _out_idx = idx; }
	void to(Node * n, int idx) { _to = n; _in_idx = idx; }
private:
	Node * _from;
	Node * _to;
	int _out_idx, _in_idx;
};

#endif /* CONNECTION_H_ */
