#pragma once

#include "render/vertex_array.h"

#include <glad/glad.h>

namespace Rainy
{
    constexpr size_t MAX_VERTEX_BUFFERS = 3;

    class OGLVertexArray : public VertexArray
    {
    public:
        OGLVertexArray();

        void SetVertexBuffers(std::initializer_list<VertexBuffer *> buffers) override;

        void SetIndexBuffer(IndexBuffer *buffer) override;

        void SetInstanceCount(uint32_t primCount) override { m_instanceCount = primCount; }

        std::vector<VertexBuffer *> GetVertexBuffers() const override;

        IndexBuffer *GetIndexBuffer() const override;

        uint32_t GetInstanceCount() const override { return m_instanceCount; }

        void Bind() override;

        void UnBind() override;

    private:
        GLuint m_index;

        GLuint m_instanceCount;

        std::vector<VertexBuffer *> m_vertexBuffers;
        IndexBuffer *m_indexBuffer;

        void (OGLVertexArray::*m_drawFun)() const;
    };

}
