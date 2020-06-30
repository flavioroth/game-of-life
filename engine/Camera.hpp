//
// Created by fla on 24.06.20.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace engine {

class Camera {
private:
	glm::vec2 _position;
	glm::mat4 _zoomMatrix;
	glm::vec2 _dragDisplacement;

public:
	Camera()
		: _position(0, 0)
		, _zoomMatrix(1.0)
		, _dragDisplacement(0, 0) {}

	void setDragDisplacement(const glm::vec2& dragDisplacement) {
		_dragDisplacement = dragDisplacement;
	}

	void integrateDisplacement() {
		_position += _dragDisplacement;
		_dragDisplacement = glm::vec2(0, 0);
	}

	void setZoom(float zoom, const glm::vec2& center) {
		_zoomMatrix = glm::translate(_zoomMatrix, glm::vec3(center, 0.0));
		_zoomMatrix = glm::scale(_zoomMatrix, glm::vec3(zoom));
		_zoomMatrix = glm::translate(_zoomMatrix, -(glm::vec3(center, 0.0)));
	}

	void zoomIn(float factor, const glm::vec2& center) {
		setZoom(factor, center);
	}

	void zoomOut(float factor, const glm::vec2& center) {
		setZoom(1.0f / factor, center);
	}

	glm::mat4 buildTransformMatrix() const {
		auto mat = glm::mat4(1.0f);
		mat = glm::translate(mat, glm::vec3(_position, 0.0));
		mat = glm::translate(mat, glm::vec3(_dragDisplacement, 0.0));
		mat *= _zoomMatrix;
		return mat;
	}
};

}
