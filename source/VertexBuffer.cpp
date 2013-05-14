#include <glow/VertexBuffer.h>
#include <glow/VertexArrayObject.h>

using namespace glow;


VertexBuffer::VertexBuffer(VertexArrayObject* vao, GLint bindingIndex)
: Buffer(GL_ARRAY_BUFFER)
, _vao(vao)
, _bindingIndex(bindingIndex)
{
}

VertexBuffer* VertexBuffer::create(VertexArrayObject* vao, GLint bindingIndex)
{
	vao->bind();
	VertexBuffer* vbo = new VertexBuffer(vao, bindingIndex);
	vao->unbind();
	return vbo;
}

GLint VertexBuffer::bindingIndex() const
{
	return _bindingIndex;
}

VertexArrayObject* VertexBuffer::vao()
{
	return _vao;
}

VertexAttributeBinding* VertexBuffer::bindToAttribute(GLint attributeLocation)
{
	VertexAttributeBinding* binding = _vao->binding(attributeLocation);
	binding->setBuffer(this);
	return binding;
}
