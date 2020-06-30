//
// Created by fla on 19.06.20.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>

namespace engine {

class Size {
private:
	glm::uvec2 _vec;

public:
	Size(uint32_t width, uint32_t height)
		: _vec(width, height)
	{}

	uint32_t width() const {
		return _vec.x;
	}

	uint32_t height() const {
		return _vec.y;
	}

	size_t area() const {
		return glm::compMul(_vec);
	}

	const glm::uvec2& vec() const {
		return _vec;
	}
};
}
