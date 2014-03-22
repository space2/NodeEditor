/*
 * Util.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef UTIL_H_
#define UTIL_H_

class Util {
public:
	static inline int abs(int v) { return v < 0 ? -v : v; }
	static inline float abs(float v) { return v < 0 ? -v : v; }
};

#endif /* UTIL_H_ */
