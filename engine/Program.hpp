//
// Created by fla on 18.06.20.
//

#pragma once

#include "GLResource.hpp"

#include "Shader.hpp"
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gl {

class Program : public GLResource {
private:
	std::unordered_map<GLint, Shader::Ptr> _shaders;

public:
	using Ptr = std::shared_ptr<Program>;

public:
	Program()
		: GLResource(glCreateProgram()) {}

public:
	template <typename... Args>
	static Ptr make(Args&&... args) {
		return std::make_shared<Program>(std::forward<Args>(args)...);
	}

	virtual ~Program() {
		glDeleteProgram(id());
	}

	Shader::Ptr addShader(Shader::Ptr shader) {
		return _shaders.emplace(shader->id(), shader).first->second;
	}

	bool removeShader(Shader::Ptr shader) {
		auto it = _shaders.find(shader->id());
		if(_shaders.end() != it) {
			_shaders.erase(it);
			return true;
		}
		return false;
	}

	bool link() {
		attachAll();
		glLinkProgram(id());
		popErrors("glLinkProgram");
		detachAll();
		return isLinked();
	}

	bool isLinked() const {
		GLint isLinked = GL_FALSE;
		glGetProgramiv(id(), GL_LINK_STATUS, &isLinked);
		return GL_TRUE == isLinked;
	}

	void use() const {
		glUseProgram(id());
		popErrors("glUseProgram");
	}

	void uniformMatrix4f(int location, const glm::mat4x4& mat) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
		popErrors("uniformMatrix4f");
	}

	void uniformMatrix3f(int location, const glm::mat3x3 &mat) {
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
		popErrors("uniformMatrix3f");
	}


	void uniform3f(int location, const glm::vec3& vec) {
		glUniform3fv(location, 1, glm::value_ptr(vec));
		popErrors("uniform2f");
	}

	void uniform2f(int location, const glm::vec2& vec) {
		glUniform2fv(location, 1, glm::value_ptr(vec));
		popErrors("uniform2f");
	}

	void uniform2u(int location, const glm::uvec2& vec) {
		glUniform2uiv(location, 1, glm::value_ptr(vec));
		popErrors("glUniform2uiv");
	}

	void uniform1f(int location, GLfloat val) {
		glUniform1f(location, val);
		popErrors("glUniform1f");
	}

	void uniform1i(int location, GLint value) {
		glUniform1i(location, value);
		popErrors("glUniform1i");
	}

	void uniform1iv(int location, const std::vector<GLint>& values) {
		glUniform1iv(location, values.size(), values.data());
		popErrors("uniform1iv");
	}

	std::string errorLog() const {
		GLint maxLength = 0;
		glGetProgramiv(id(), GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::string errorLog(maxLength, '\0');
		glGetProgramInfoLog(id(), maxLength, &maxLength, (GLchar*) errorLog.data());

		return errorLog;
	}

private:
	void attachAll() {
		for(const auto& shader : _shaders) {
			glAttachShader(id(), shader.second->id());
			popErrors("glAttachShader");
		}
	}

	void detachAll() {
		for(const auto& shader : _shaders) {
			glDetachShader(id(), shader.second->id());
			popErrors("glDetachShader");
		}
	}
};

}// namespace gl
