/*
 * Slot.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef SLOT_H_
#define SLOT_H_

class Slot {
public:
	enum Type {
		Bit = 0,
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
	Slot(Type type, const char * name);
	~Slot();

	const Type type() const { return _type; }
	const char * name() const { return _name; }
private:
	Type _type;
	const char * _name;
};

#endif /* SLOT_H_ */
