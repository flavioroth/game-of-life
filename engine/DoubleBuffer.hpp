//
// Created by fla on 19.06.20.
//

#pragma once

#include <array>

namespace engine {

template <typename T>
class DoubleBuffer {
private:
	std::array<T, 2> _items;
	std::array<std::reference_wrapper<T>, 2> _pointers;

public:

	template <typename... Args>
	DoubleBuffer(Args&&... args)
		: _items {T(std::forward<Args>(args)...), T(std::forward<Args>(args)...)}
		, _pointers {_items[0], _items[1]}
	{}

	T& first() {
		return _pointers[0];
	}

	T& second() {
		return _pointers[1];
	}

	void flip() {
		std::swap(_pointers[0], _pointers[1]);
	}
};
}
