#pragma once

#include <glow/glow.h>
#include <glow/Object.h>

namespace glow {

/**
 * \brief The RenderBufferObject class encapsulates OpenGL render buffer objects.
 *
 * \see http://www.opengl.org/wiki/Renderbuffer_Objects
 */
class GLOW_API RenderBufferObject : public Object
{
public:
	RenderBufferObject();
	virtual ~RenderBufferObject();

	virtual void accept(ObjectVisitor& visitor);

	void bind();
	void unbind();

	void storage(GLenum internalformat, GLsizei width, GLsizei height);
	void storageMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

	GLint getParameter(GLenum pname);
protected:
	static GLuint genRenderBuffer();
};

} // namespace glow
