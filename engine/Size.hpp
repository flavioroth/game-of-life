//
// Created by fla on 19.06.20.
//

#pragma once

#include <cstddef>
#include <cstdint>

namespace engine {

class Size {
private:
	uint32_t _width;
	uint32_t _height;

public:
	Size(uint32_t width, uint32_t height)
		: _width(width)
		, _height(height)
	{}

	uint32_t width() const {
		return _width;
	}

	uint32_t height() const {
		return _height;
	}

	size_t area() const {
		return width() * height();
	}
};
}
