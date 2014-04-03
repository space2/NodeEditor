/*
 * Slot.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include <pugixml/pugixml.hpp>

class Slot {
public:
	enum Type {
		Undefined = 0,
		Bit,
		Int,
		Float,
		String,
		BitVec,
		IntVec,
		FloatVec,
		BitMat,
		IntMat,
		FloatMat,
		Image,
	};
	Slot(const char * name);
	~Slot();

	const Type type() const { return _type; }
	const char * name() const { return _name; }
	void name(const char * name);

	int as_bit() const { return _int_val; }
	int as_int() const { return _int_val; }
	int as_float() const { return _float_val; }

	int equals(const Slot * other);

	void set(const Slot * other);
	void set_undefined();
	void set_bit(int bit);

	int changed() const { return _changed; }
	void changed(int v) { _changed = v; }

	void save_to(pugi::xml_node & node, const char * name);
	void load_from(pugi::xml_node & node, const char * name);

private:
	Type _type;
	char * _name;
	int _changed;
	int _int_val;
	float _float_val;
};

