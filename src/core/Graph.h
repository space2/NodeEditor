/*
 * Graph.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include "Group.h"

class Graph : public Group {
public:
	Graph();
	virtual ~Graph();

	int save_to_file(const char * fn);
	int load_from_file(const char * fn);

	const char * file_name() const { return _file_name; }

private:
	void file_name(const char * fn);

	char * _file_name;
};

