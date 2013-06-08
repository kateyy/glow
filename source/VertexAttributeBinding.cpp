#include <glow/VertexAttributeBinding.h>
#include <glow/VertexArrayObject.h>

using namespace glow;

VertexAttributeBinding::VertexAttributeBinding(VertexArrayObject* vao, GLint attributeIndex)
: _vao(vao)
, _attributeIndex(attributeIndex)
, _vbo(nullptr)
{
	_implementation = new VertexAttributeBinding_GL_3_2(this);
	//_implementation = new VertexAttributeBinding_GL_4_3(this);
}

VertexAttributeBinding::~VertexAttributeBinding()
{
	delete _implementation;
}

GLint VertexAttributeBinding::attributeIndex() const
{
	return _attributeIndex;
}

VertexArrayObject* VertexAttributeBinding::vao() const
{
	return _vao;
}

VertexBuffer* VertexAttributeBinding::vbo() const
{
	return _vbo;
}

void VertexAttributeBinding::setBuffer(VertexBuffer* vbo, GLint baseoffset, GLint stride)
{
	_vbo = vbo;
	_implementation->bindBuffer(vbo, baseoffset, stride);
}

void VertexAttributeBinding::enable()
{
	_vao->bind();
	glEnableVertexAttribArray(_attributeIndex);
}

void VertexAttributeBinding::disable()
{
	_vao->bind();
	glDisableVertexAttribArray(_attributeIndex);
}

void VertexAttributeBinding::setFormat(GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
{
	_implementation->setFormat(size, type, normalized, relativeoffset);
}

void VertexAttributeBinding::setIFormat(GLint size, GLenum type, GLuint relativeoffset)
{
	_implementation->setIFormat(size, type, relativeoffset);
}

void VertexAttributeBinding::setLFormat(GLint size, GLuint relativeoffset)
{
	_implementation->setLFormat(size, GL_DOUBLE, relativeoffset);
}


VertexAttributeBindingImplementation::VertexAttributeBindingImplementation(VertexAttributeBinding* binding)
: _binding(binding)
{
}

VertexAttributeBindingImplementation::~VertexAttributeBindingImplementation()
{
}

// GL 3.2 implementation

VertexAttributeBinding_GL_3_2::VertexAttributeBinding_GL_3_2(VertexAttributeBinding* binding)
: VertexAttributeBindingImplementation(binding)
, _baseoffset(0)
, _stride(0)
{
}

void VertexAttributeBinding_GL_3_2::bindBuffer(VertexBuffer* vbo, GLint baseoffset, GLint stride)
{
	_baseoffset = baseoffset;
	_stride = stride;
	if (_format.initialized)
	{
		bindIndex();
	}
}

void VertexAttributeBinding_GL_3_2::setFormat(GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
{
	_format = Format(Format::O, size, type, normalized, relativeoffset);
	bindIndex();
}

void VertexAttributeBinding_GL_3_2::setIFormat(GLint size, GLenum type, GLuint relativeoffset)
{
	_format = Format(Format::I, size, type, GL_FALSE, relativeoffset);
	bindIndex();
}

void VertexAttributeBinding_GL_3_2::setLFormat(GLint size, GLenum type, GLuint relativeoffset)
{
	_format = Format(Format::L, size, type, GL_FALSE, relativeoffset);
	bindIndex();
}

VertexAttributeBinding_GL_3_2::Format::Format()
: method(O)
, size(0)
, type(0)
, normalized(GL_FALSE)
, relativeoffset(0)
, initialized(false)
{
}

VertexAttributeBinding_GL_3_2::Format::Format(Method method, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
: method(method)
, size(size)
, type(type)
, normalized(normalized)
, relativeoffset(relativeoffset)
, initialized(true)
{
}

void VertexAttributeBinding_GL_3_2::bindIndex()
{
	if (!_binding->vbo()) return;


	_binding->vao()->bind();
	_binding->vbo()->bind(GL_ARRAY_BUFFER);
	switch (_format.method)
	{
		case Format::I:
			glVertexAttribIPointer(_binding->attributeIndex(), _format.size, _format.type, _stride, reinterpret_cast<void*>(_baseoffset+_format.relativeoffset));
			break;
		case Format::L:
			glVertexAttribLPointer(_binding->attributeIndex(), _format.size, _format.type, _stride, reinterpret_cast<void*>(_baseoffset+_format.relativeoffset));
			break;
		default:
			glVertexAttribPointer(_binding->attributeIndex(), _format.size, _format.type, _format.normalized, _stride, reinterpret_cast<void*>(_baseoffset+_format.relativeoffset));
	}
}

// GL 4.3 implementation

#ifdef GL_VERSION_4_3

VertexAttributeBinding_GL_4_3::VertexAttributeBinding_GL_4_3(VertexAttributeBinding* binding)
: VertexAttributeBindingImplementation(binding)
{
}

void VertexAttributeBinding_GL_4_3::bindBuffer(VertexBuffer* vbo, GLint baseoffset, GLint stride)
{
	_binding->vao()->bind();

	glVertexAttribBinding(_binding->attributeIndex(), vbo->bindingIndex());

	_binding->vbo()->bindVertexBuffer(baseoffset, stride);
}

void VertexAttributeBinding_GL_4_3::setFormat(GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
{
	_binding->vao()->bind();

	glVertexAttribFormat(_binding->attributeIndex(), size, type, normalized, relativeoffset);
}

void VertexAttributeBinding_GL_4_3::setIFormat(GLint size, GLenum type, GLuint relativeoffset)
{
	_binding->vao()->bind();

	glVertexAttribIFormat(_binding->attributeIndex(), size, type, relativeoffset);
}

void VertexAttributeBinding_GL_4_3::setLFormat(GLint size, GLenum type, GLuint relativeoffset)
{
	_binding->vao()->bind();

	glVertexAttribLFormat(_binding->attributeIndex(), size, type, relativeoffset);
}

#endif
