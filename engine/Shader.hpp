//
// Created by fla on 18.06.20.
//

#pragma once


#include "GLResource.hpp"

#include <memory>
#include <string>
#include <vector>

namespace gl {

class Shader : public GLResource {
public:
	using Ptr = std::shared_ptr<Shader>;

public:
	Shader(GLenum type)
		: GLResource(glCreateShader(type)) {}

	Shader(GLenum type, const std::vector<std::string>& sources)
			: Shader(type) {
		setSources(sources);
	}

	Shader(GLenum type, const std::string& source)
			: Shader(type, std::vector<std::string>{source}) {
	}

	template <typename ...Args>
	static Ptr make(Args&& ...args) {
		return std::make_shared<Shader>(std::forward<Args>(args)...);
	}

	virtual ~Shader() {
		glDeleteShader(id());
	}

	bool isCompiled() const {
		GLint isCompiled = 0;
		glGetShaderiv(id(), GL_COMPILE_STATUS, &isCompiled);
		return GL_TRUE == isCompiled;
	}

	bool compile() {
		glCompileShader(id());
		popErrors("glCompileShader");
		return isCompiled();
	}

	void setSource(const std::string& source){
		setSources({source});
	}

	void setSources(const std::vector<std::string>& sources) {
		std::vector<GLint> lengths;
		std::vector<const char*> strings;
		lengths.reserve(sources.size());
		strings.reserve(sources.size());
		for(const auto& src : sources) {
			lengths.push_back(src.size());
			strings.push_back(src.data());
		}

		glShaderSource(id(), sources.size(), strings.data(), lengths.data());
		popErrors("glShaderSource");
	}

	std::string getErrorLog() const {
		GLint maxLength = 0;
		glGetShaderiv(id(), GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::string errorLog(maxLength, '\0');
		glGetShaderInfoLog(id(), maxLength, &maxLength, (GLchar*) errorLog.data());

		return errorLog;
	}
};

}// namespace gl
