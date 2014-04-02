/*
 * Util.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

class Util {
public:
	static inline int abs(int v) { return v < 0 ? -v : v; }
	static inline float abs(float v) { return v < 0 ? -v : v; }
	static inline int min(int a, int b) { return a < b ? a : b; }
	static inline int max(int a, int b) { return a > b ? a : b; }
	static inline float min(float a, float b) { return a < b ? a : b; }
	static inline float max(float a, float b) { return a > b ? a : b; }
	static inline void swap(int & a, int & b) { int tmp = a; a = b; b = tmp; }
	static inline void swap(float & a, float & b) { float tmp = a; a = b; b = tmp; }

	static inline const char * bit2string(int bit) {
		if (bit == 0) return "0";
		if (bit == 1) return "1";
		return "?";
	}
};

