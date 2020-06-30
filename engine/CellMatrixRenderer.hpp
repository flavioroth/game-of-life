//
// Created by fla on 19.06.20.
//

#pragma once

#include "CellMatrix.hpp"
#include "Texture.hpp"

#define LITERAL_GLSL(...) #__VA_ARGS__

namespace renderer {

class CellMatrixRenderer {
private:
	gl::Program::Ptr _program;
	gl::Texture::Ptr _texture;

	const char* _vertex_src = LITERAL_GLSL(
		\x23 version 330\n
		\x23 extension GL_ARB_explicit_uniform_location
		: require\n
		out vec2 vUV;
		const vec2 data[4] = vec2[](vec2(-1.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(1.0, -1.0));

		void main() {
			vec4 vertex = vec4(data[gl_VertexID], 0.0, 1.0);
			vUV = vec2(1.0, -1.0) * (vertex.xy * 0.5 + 0.5);
			gl_Position = vertex;
		}
	);

	const char* _frag_src = LITERAL_GLSL(
		\x23 version 330\n
		\x23 extension GL_ARB_explicit_uniform_location: require\n
		in vec2 vUV;
		layout(location = 0) uniform mat4 uViewMatrix;
		layout(location = 1) uniform uvec2 uDimensions;
		layout(location = 2) uniform sampler2D uCellsTexture;
		out vec4 fragColor;
		void main() {
			vec2 uv = gl_FragCoord.xy / uDimensions.xy;
			uv = (uViewMatrix * vec4(uv.x, uv.y, 0.0, 1.0)).xy;
			vec4 color = texture2D(uCellsTexture, uv);
			float luminance = min(1.0, color.r * 255.0);
			fragColor = vec4(luminance, luminance, luminance, 1.0);
		}
	);


public:
	CellMatrixRenderer()
		: _program(gl::Program::make())
		, _texture(gl::Texture::make()) {

		_program
			->addShader(gl::Shader::make(GL_FRAGMENT_SHADER, _frag_src))
			->compile();

		_program
			->addShader(gl::Shader::make(GL_VERTEX_SHADER, _vertex_src))
			->compile();

		if(!_program->link()) {
			fprintf(stderr, "Error linking program: %s", _program->errorLog().c_str());
		}

		_program->use();

		// set the image texture up
		_texture->bindToTextureUnit(0, GL_TEXTURE_2D);

		// set filtering
		_texture->setParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_texture->setParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// assign the texture to the fragment shader
		_program->uniform1i(2, 0);
	}

	void prepare(const engine::Size& gridDimensions, const glm::mat4& viewMatrix) {

		// update the view matrix
		_program->uniformMatrix4f(0, viewMatrix);

		//mProjMatrix = glm::scale(mProjMatrix, glm::vec3(0.999));
		//mProjMatrix = glm::rotate(mProjMatrix, 0.001f, glm::vec3(0.0, 0.0, 1.0));
		//mProjMatrix = glm::translate(mProjMatrix, glm::vec3(0.001, 0.02, 0.0));

		// update dimensions
		_program->uniform2u(1, gridDimensions.vec());
	}

	void render(const engine::CellMatrix<uint8_t>& cellMatrix) {
		glTexImage2D(_texture->boundTarget(),
			0,
			GL_R8,
			cellMatrix.size().width(),
			cellMatrix.size().height(),
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			cellMatrix.data()
		);
		gl::GLResource::popErrors("glTexImage2D");
	}
};

}
