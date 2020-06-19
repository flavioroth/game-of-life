//
// Created by fla on 18.06.20.
//

#pragma once

#include "GLResource.hpp"
#include <memory>

namespace gl {

class Texture : public GLResource {
public:
	using Ptr = std::shared_ptr<Texture>;

	struct Format {
		const GLsizei width;
		const GLsizei height;
		const GLint internalFormat;
		const GLenum format;
		const GLenum pixelDataType;
	};

	enum class Filtering { Nearest, Linear };

private:
	GLenum _bindTarget;
	Format _format;

public:
	Texture()
		: GLResource(generateTexture())
		, _format{0, 0, 0, 0, 0} {
	}

	template <typename... Args>
	static Ptr make(Args&&... args) {
		return std::make_shared<Texture>(std::forward<Args>(args)...);
	}

	virtual ~Texture() {
		glDeleteTextures(1, &_id);
		popErrors("glDeleteTextures");
	}

	void setFormat(GLint level, GLsizei width, GLsizei height, GLint internalFormat, GLenum format, GLenum pixelDataType) {
		glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, format, pixelDataType, nullptr);
		popErrors("setFormat:glTexImage2D");
	}

	const Format& format() const {
		return _format;
	}

	void setBindTarget(GLenum target) {
		glBindTexture(target, id());
		popErrors("glBindTexture");
		_bindTarget = target;
	}

	void unbind() {
		glBindTexture(boundTarget(), 0);
		popErrors("glBindTexture (unbind)");
		_bindTarget = 0;
	}

	GLenum boundTarget() const {
		return _bindTarget;
	}

	static void activateTextureUnit(int unit) {
		glActiveTexture(GL_TEXTURE0 + unit);
		popErrors("glActiveTexture");
	}

	static void setParameteri(GLenum target, GLenum name, GLint param) {
		glTexParameteri(target, name, param);
		popErrors("glTextureParameteri");
	}

	static void setParameteriv(GLenum target, GLenum name, const GLint* params) {
		glTexParameteriv(target, name, params);
		popErrors("glTextureParameteriv");
	}

	void setFiltering(Filtering filtering) {
		if(Filtering::Linear == filtering) {
			setParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			setParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else if(Filtering::Nearest == filtering) {
			setParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			setParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

private:
	static GLuint generateTexture() {
		GLuint id;
		glGenTextures(1, &id);
		popErrors("glGenTextures");
		return id;
	}

};

}

