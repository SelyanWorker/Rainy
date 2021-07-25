#include "OGLBuffer.h"

namespace Rainy {


	// vertex buffer
	VertexBuffer* VertexBuffer::Create(uint32_t size, void* data)
	{
		return new OGLVertexBuffer(size, data);
	}

	OGLVertexBuffer::OGLVertexBuffer(uint32_t size, void* data)
	:	m_size(size)
	{
		glGenBuffers(1, &m_index);
		glBindBuffer(GL_ARRAY_BUFFER, m_index);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OGLVertexBuffer::~OGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_index);
	}

	void OGLVertexBuffer::SetBufferLayout(BufferLayout layout)
	{
		m_layout = layout;
	}

	BufferLayout const& OGLVertexBuffer::GetBufferLayout() const
	{
		return m_layout;
	}

	void OGLVertexBuffer::SubData(int64_t offset, int64_t size, void* data)
	{
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		UnBind();
	}

	void OGLVertexBuffer::Reallocate(uint32_t size, void* data)
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		m_size = size;
		UnBind();
	}

	void OGLVertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_index);
	}

	void OGLVertexBuffer::UnBind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// index buffer
	IndexBuffer* IndexBuffer::Create(uint32_t size, uint32_t count, void* data)
	{
		return new OGLIndexBuffer(size, count, data);
	}

	OGLIndexBuffer::OGLIndexBuffer(uint32_t size, uint32_t count, void* data)
	:	m_size(size),
		m_count(count)
	{
		glGenBuffers(1, &m_index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OGLIndexBuffer::~OGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_index);
	}

	uint32_t OGLIndexBuffer::GetCount() const
	{
		return m_count;
	}

	void OGLIndexBuffer::SubData(int64_t offset, int64_t size, void* data)
	{
		Bind();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
		UnBind();
	}

	void OGLIndexBuffer::Reallocate(uint32_t size, void* data, uint32_t count)
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_ELEMENT_ARRAY_BUFFER);
		if (count != 0) m_count = count;
		UnBind();
	}

	void OGLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
	}

	void OGLIndexBuffer::UnBind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// shader buffer
	ShaderBuffer* ShaderBuffer::Create(uint32_t size, void* data)
	{
		return new OGLShaderBuffer(size, data);
	}

	OGLShaderBuffer::OGLShaderBuffer(uint32_t size, void* data)
	:	m_size(size)
	{
		glGenBuffers(1, &m_index);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_index);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	OGLShaderBuffer::~OGLShaderBuffer()
	{
		glDeleteBuffers(1, &m_index);
	}

	void* OGLShaderBuffer::GetData(uint32_t offset, uint32_t size) const
	{
		uint8_t* data = new uint8_t[size];
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		return data;
	}

	void OGLShaderBuffer::SubData(int64_t offset, int64_t size, void* data)
	{
		Bind();
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		UnBind();
	}

	void OGLShaderBuffer::Reallocate(uint32_t size, void* data)
	{
		Bind();
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
		m_size = size;
		UnBind();
	}

	void OGLShaderBuffer::Bind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_index);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_index);
	}

	void OGLShaderBuffer::UnBind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

}
