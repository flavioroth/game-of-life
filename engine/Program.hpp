//
// Created by fla on 18.06.20.
//

#pragma once

#include "GLResource.hpp"


#include "Shader.hpp"
#include <unordered_map>

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

	bool addShader(Shader::Ptr shader) {
		if(_shaders.end() == _shaders.find(shader->id())) {
			_shaders.emplace(shader->id(), shader);
			return true;
		}
		return false;
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
