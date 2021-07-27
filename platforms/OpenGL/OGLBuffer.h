#pragma once

#include "render/buffer.h"

#include <glad/glad.h>

namespace Rainy {

	class OGLVertexBuffer : public VertexBuffer
	{
	public:
		OGLVertexBuffer(uint32_t size, void* data = nullptr);

		~OGLVertexBuffer();

		void SetBufferLayout(BufferLayout layout) override;

		BufferLayout const& GetBufferLayout() const override;

		void SubData(int64_t offset, int64_t size, void* data) override;

		void Reallocate(uint32_t size, void* data) override;

		void Bind() override;

		void UnBind() override;

	private:
		GLsizeiptr m_size;
		GLuint m_index;

		BufferLayout m_layout;
	};

	class OGLIndexBuffer : public IndexBuffer
	{
	public:
		OGLIndexBuffer(uint32_t size, uint32_t count, void* data = nullptr);

		~OGLIndexBuffer();

		uint32_t GetCount() const override;

		void SubData(int64_t offset, int64_t size, void* data) override;

		void Reallocate(uint32_t size, void* data, uint32_t count = 0) override;

		void Bind() override;

		void UnBind() override;

	private:
		GLsizeiptr m_size;
		GLsizei m_count;
		GLuint m_index;
	};

	class OGLShaderBuffer : public ShaderBuffer
	{
	public:
		OGLShaderBuffer(uint32_t size, void* data = nullptr);

		~OGLShaderBuffer();

		void* GetData(uint32_t offset, uint32_t size) const override;

		void SubData(int64_t offset, int64_t size, void* data) override;

		void Reallocate(uint32_t size, void* data) override;

		void Bind() override;

		void UnBind() override;

	private:
		GLsizeiptr m_size;
		GLsizei m_count;
		GLuint m_index;
	};

}
