#include "OGLVertexArray.h"

namespace Rainy
{
    VertexArray *VertexArray::Create() { return new OGLVertexArray(); }

    OGLVertexArray::OGLVertexArray() : m_index(0), m_indexBuffer(nullptr), m_instanceCount(1)
    {
        glGenVertexArrays(1, &m_index);
    }

    void OGLVertexArray::SetVertexBuffers(std::initializer_list<VertexBuffer *> buffers)
    {
        RN_ASSERT(buffers.size() <= MAX_VERTEX_BUFFERS, "buffers.size() > m_vertexBuffers.size()");
        Bind();
        GLuint index = 0;
        for (auto initBuffer : buffers)
        {
            if (initBuffer == nullptr)
                continue;

            m_vertexBuffers.push_back(initBuffer);

            VertexBuffer *buffer = m_vertexBuffers.back();
            buffer->Bind();
            auto bufferLayout = buffer->GetBufferLayout();
            GLuint stride = bufferLayout.GetStride();
            for (auto const &element : bufferLayout)
            {
                GLenum type;
                GLboolean normalize = element.Normalize ? GL_TRUE : GL_FALSE;
                switch (element.Type)
                {
                    case FLOAT2:
                        type = GL_FLOAT;
                        break;
                    case FLOAT3:
                        type = GL_FLOAT;
                        break;
                    case NONE:
                        RN_CORE_ERROR("SetVertexBuffer element no type");
                    default:
                        RN_CORE_ERROR("From SetVertexBuffers -> default in type switch");
                }
                glVertexAttribPointer(index,
                                      element.Size,
                                      type,
                                      normalize,
                                      stride,
                                      (void *)element.Offset);
                glEnableVertexAttribArray(index);
                glVertexAttribDivisor(index, GLuint(element.Divisor));
                index++;
            }
            buffer->UnBind();
        }
        UnBind();
    }

    void OGLVertexArray::SetIndexBuffer(IndexBuffer *buffer)
    {
        m_indexBuffer = buffer;
        Bind();
        m_indexBuffer->Bind();
        UnBind();
    }

    std::vector<VertexBuffer *> OGLVertexArray::GetVertexBuffers() const { return m_vertexBuffers; }

    IndexBuffer *OGLVertexArray::GetIndexBuffer() const { return m_indexBuffer; }

    void OGLVertexArray::Bind() { glBindVertexArray(m_index); }

    void OGLVertexArray::UnBind() { glBindVertexArray(0); }

}
