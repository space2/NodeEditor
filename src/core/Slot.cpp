/*
 * Slot.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <string.h>
#include <stdlib.h>

#include "Slot.h"

Slot::Slot(const char * name)
	: _type(Undefined), _name(strdup(name)), _changed(0), _int_val(0), _float_val(0)
{
}

Slot::~Slot()
{
	free(_name);
}

void Slot::name(const char * name)
{
	free(_name);
	_name = strdup(name);
}

int Slot::equals(const Slot * other)
{
	if (_type != other->_type) return 0;
	if (_type == Bit) return _int_val == other->_int_val;
	return 0;
	// TODO
}

void Slot::set(const Slot * other)
{
	if (equals(other)) return;
	_type = other->_type;
	_int_val = other->_int_val;
	_float_val = other->_float_val;
	_changed = 1;
	// TODO: handle other format
}

void Slot::set_bit(int bit)
{
	if (_type == Bit && _int_val == bit) return;
	_type = Bit;
	_int_val = bit;
	_changed = 1;
}

void Slot::set_undefined()
{
	if (_type == Undefined) return;
	_type = Undefined;
	_changed = 1;
}

void Slot::save_to(pugi::xml_node & node, const char * name)
{
	pugi::xml_node child = node.append_child(name);
	switch (_type) {
	case Undefined:
		child.append_attribute("type").set_value("undefined");
		break;
	case Bit:
		child.append_attribute("type").set_value("bit");
		child.append_attribute("value").set_value(_int_val);
		break;
	default:
		child.append_attribute("type").set_value("unknown");
		break;
	}
}

void Slot::load_from(pugi::xml_node & node, const char * name)
{
	for (pugi::xml_node child = node.first_child(); node; node = node.next_sibling()) {
		if (0 == strcmp(name, child.name())) {
			const char * type = child.attribute("type").as_string();
			if (0 == strcmp(type, "bit")) {
				set_bit(_int_val = child.attribute("value").as_int());
			} else {
				set_undefined();
			}
			break;
		}
	}
}

