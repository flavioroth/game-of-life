//
// Created by fla on 19.06.20.
//

#pragma once

#include "CellMatrix.hpp"
#include "Texture.hpp"

namespace renderer {

class CellMatrixRenderer {
public:
	static void render(const engine::CellMatrix<uint8_t>& cellmatrix, GLenum target) {
		glTexImage2D(target,
			0,
			GL_R8,
			cellmatrix.size().width(),
			cellmatrix.size().height(),
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			cellmatrix.data()
		);
		gl::GLResource::popErrors("glTexImage2D");
	}
};

}
