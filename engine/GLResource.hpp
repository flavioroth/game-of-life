//
// Created by fla on 18.06.20.
//

#pragma once


#include <GL/glu.h>

#include <cstdio>
#include <exception>
#include <sstream>
#include <vector>
#include <string>

namespace gl {

class GLException : public std::exception {
private:
	std::string _what;

public:
	GLException(const std::vector<GLenum>& errorCodes, std::string context) {

		std::stringstream message;
		message << errorCodes.size() << " error(s) : " << std::endl;
		for(GLenum code : errorCodes) {
			message << "GL error code = " << code
					<< " message = " << gluErrorString(code)
					<< " in " << context
					<< std::endl;
		}
		message << std::endl;

		_what = message.str();
	}

	const char* what() const throw() {
		return _what.c_str();
	}
};


class GLResource {
protected:
	const GLuint _id;

public:

	static void popErrors(const char* ctx = "") {
		GLenum err = GL_NO_ERROR;
		std::vector<GLenum> errorCodes;
		while((err = glGetError()) != GL_NO_ERROR) { errorCodes.push_back(err); }

		if(!errorCodes.empty()) {
			throw GLException(errorCodes, ctx);
		}
	}


	GLResource(GLint id)
		: _id(id) {
		popErrors(__PRETTY_FUNCTION__);
	}

	GLint id() const {
		return _id;
	}

};

}// namespace gl
