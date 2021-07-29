#include "render/buffer.h"
#include "render/renderer.h"

#include <glad/glad.h>

namespace Rainy
{
    void Setup()
    {
        glClearColor(0.8f, 0.8f, 0.8f, 1.f);
        glEnable(GL_DEPTH_TEST);
    }

    void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

    uint32_t CreateBuffer()
    {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        return vbo;
    }

    void BufferData(uint32_t buffer, void *dataPtr, size_t dataSize)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, dataSize, dataPtr, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void BufferSubData(uint32_t buffer, int64_t offset, int64_t size, void *data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void IndicesBufferData(uint32_t buffer, void *dataPtr, size_t dataSize)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, dataPtr, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndicesBufferSubData(uint32_t buffer, int64_t offset, int64_t size, void *data)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    uint32_t CreateVertexArray()
    {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        return vao;
    }

    void BufferAttach(uint32_t vertexArray,
                      uint32_t buffer,
                      uint32_t index,
                      uint32_t size,
                      uint32_t stride,
                      void *pointer)
    {
        RN_ASSERT(vertexArray != 0 && buffer != 0,
                  "VertexArray == 0 or Buffer == 0 in \"BufferAttach\"");
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
        glEnableVertexAttribArray(index);
        glBindVertexArray(0);
    }

    void IndicesBufferAttach(uint32_t vertexArray, uint32_t buffer)
    {
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBindVertexArray(0);
    }

    void DrawArray(RenderMode mode, uint32_t vertexArray, uint32_t count)
    {
        glBindVertexArray(vertexArray);
        glDrawArrays(mode, 0, count);
        glBindVertexArray(0);
    }

    void DrawElements(RenderMode mode, uint32_t vertexArray, uint32_t count)
    {
        glBindVertexArray(vertexArray);
        glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void EnableRestartIndex(uint32_t index)
    {
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(index);
    }

    void DisableRestartIndex() { glDisable(GL_PRIMITIVE_RESTART); }

    void EnablePolygonMode() { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }

    void DisablePolygonMode() { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

    void EnableCullFace() { glEnable(GL_CULL_FACE); }

    void DisableCullFace() { glDisable(GL_CULL_FACE); }

    void SetPointSize(float size) { glPointSize(size); }

    void SetLineWidth(float width) { glLineWidth(width); }

    float GetPointSize()
    {
        GLfloat size;
        glGetFloatv(GL_POINT_SIZE, &size);
        return size;
    }

    float GetLineWidth()
    {
        GLfloat width;
        glGetFloatv(GL_LINE_WIDTH, &width);
        return width;
    }

    void DeleteBuffer(uint32_t buffer) { glDeleteBuffers(1, &buffer); }

    void RendererTest()
    {
        GLboolean res;
        glGetBooleanv(GL_DEPTH_TEST, &res);
        // RN_CORE_INFO("RendererTest GL_DEPTH_TEST enable: {0}", res);
    }

    // new
    void DrawVertexArray(RenderMode mode, VertexArray *vertexArray)
    {
        vertexArray->Bind();

        if (vertexArray->GetIndexBuffer() != nullptr)
            glDrawElementsInstanced(mode,
                                    vertexArray->GetIndexBuffer()->GetCount(),
                                    GL_UNSIGNED_INT,
                                    nullptr,
                                    vertexArray->GetInstanceCount());
        else
        {
            GLuint count = vertexArray->GetVertexBuffers()[0]->GetBufferLayout().GetVertexCount();
            glDrawArraysInstanced(mode, 0, count, vertexArray->GetInstanceCount());
        }

        vertexArray->UnBind();
    }
}
