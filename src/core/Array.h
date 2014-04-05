#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

template<typename T>
class Array {
private:
	int _size;
	int _count;
	T * _data;
protected:
	virtual void _free(T item) {
	}
public:
	Array() : _size(8), _count(0), _data(NULL)
	{
		_data = (T*) calloc(_size, sizeof(T));
	}

	virtual ~Array() {
		for (int i = 0; i < _count; i++) {
			_free(_data[i]);
		}
		free(_data);
	}

	void resize(int size) {
		_size *= size;
		_data = (T*) realloc(_data, sizeof(T) * _size);
	}

	void clear() {
		for (int i = 0; i < _count; i++) {
			_free(_data[i]);
		}
		_count = 0;
	}

	int count() const {
		return _count;
	}

	T * data() {
		return _data;
	}

	T & get(int idx) {
		return _data[idx];
	}

	T & operator[](int idx) {
		return _data[idx];
	}

	const T & operator[](int idx) const {
		return _data[idx];
	}

	void set(int idx, T v) {
		if (idx >= 0 && idx < _count) {
			_free(_data[idx]);
			_data[idx] = v;
		}
	}

	int add(T data) {
		if (_count == _size) {
			_size *= 2;
			_data = (T*) realloc(_data, sizeof(T) * _size);
		}
		_data[_count++] = data;
		return _count-1;
	}

	int remove(T data, int keep = 0) {
		for (int i = 0; i < _count; i++) {
			if (data == _data[i]) {
				return remove_at(i, keep);
			}
		}
		return 0;
	}

	int indexof(T data) {
		for (int i = 0; i < _count; i++) {
			if (data == _data[i]) {
				return i;
			}
		}
		return -1;
	}

	int contains(T data) {
		return 0 <= indexof(data);
	}

	int remove_at(int idx, int keep = 0) {
		if (idx < 0 || idx >= _count) return 0;
		if (!keep) _free(_data[idx]);
		_count--;
		for (int i = idx; i < _count; i++) {
			_data[i] = _data[i+1];
		}
		return 1;
	}

	void sort(int (*f)(T lhs, T rhs)) {
		for (int i = 0; i < _count - 1; i++) {
			for (int j = i + 1; j < _count; j++) {
				int ret = f(_data[i], _data[j]);
				if (ret > 0) {
					T tmp = _data[i];
					_data[i] = _data[j];
					_data[j] = tmp;
				}
			}
		}
	}

	void swap(int idx0, int idx1) {
		T tmp = _data[idx0];
		_data[idx0] = _data[idx1];
		_data[idx1] = tmp;
	}

};

template<typename T>
class ArrayO : public Array<T>
{
protected:
	virtual void _free(T item) {
		delete item;
	}
public:
	ArrayO() {}
	virtual ~ArrayO() {}
};

template<typename T>
class ArrayP : public Array<T>
{
protected:
	virtual void _free(T item) {
		if (item) free(item);
	}
public:
	ArrayP() {}
	virtual ~ArrayP() {}
};

