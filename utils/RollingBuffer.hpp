//
// Created by fla on 18.06.20.
//

#pragma once

namespace utils {

template <size_t Size, typename TVal>
class RollingBuffer {
private:
	TVal _values[Size];

public:
	RollingBuffer()
		: _values{0} {}

	void push(TVal value) {
		memmove(&_values[0], &_values[1], (Size - 1) * sizeof(TVal));
		_values[(Size - 1)] = std::move(value);
	}

	size_t size() const {
		return Size;
	}

	const TVal* values() const {
		return &_values[0];
	}
};

}// namespace utils
