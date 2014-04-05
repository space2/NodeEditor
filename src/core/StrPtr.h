/*
 * StrPtr.h
 *
 *  Created on: Apr 5, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include <string.h>
#include <stdlib.h>

class StrPtr {
public:
	StrPtr(const char * s = 0)
		: _v(NULL)
	{
		set(s);
	}

	~StrPtr()
	{
		set(NULL);
	}

	int empty() const
	{
		return _v == NULL;
	}


	StrPtr & operator = (const char * s) {
		set(s);
		return *this;
	}

	void set(const char * s)
	{
		if (_v) free(_v);
		_v = s ? strdup(s) : NULL;
	}

	operator const char * () const
	{
		return _v;
	}

	const char * get() const {
		return _v;
	}

private:
	char * _v;
};

