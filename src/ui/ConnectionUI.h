/*
 * ConnectionUI.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef CONNECTIONUI_H_
#define CONNECTIONUI_H_

#include "core/Connection.h"

#include "NodeUI.h"

class ConnectionUI {
public:
	ConnectionUI(Connection * conn);
	virtual ~ConnectionUI();

	Connection * conn() { return _conn; }
private:
	Connection * _conn;
};

#endif /* CONNECTIONUI_H_ */
