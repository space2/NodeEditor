/*
 * ConnectionUI.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

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
