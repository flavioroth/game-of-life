//
// Created by fla on 19.06.20.
//

#pragma once

#include "Size.hpp"

#include <vector>

namespace engine {

template <typename TCell>
class CellMatrix {
private:
	std::vector<TCell> _cells;
	const Size _size;

public:
	CellMatrix(Size size)
		: _cells(size.area(), 0)
		, _size(std::move(size))
	{}

	const Size& size() const {
		return _size;
	}

	auto begin() {
		return _cells.begin();
	}

	auto end() {
		return _cells.end();
	}

	TCell& at(int32_t x, int32_t y) {
		return _cells.at((y % _size.height()) * _size.height() + (x % _size.width()));
	}

	const TCell& at(int32_t x, int32_t y) const {
		return _cells.at((y % _size.height()) * _size.height() + (x % _size.width()));
	}

	const TCell* data() const {
		return _cells.data();
	}
};

}
