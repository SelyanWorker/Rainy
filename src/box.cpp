#include "glad/glad.h"

#include "geometry/3d/box.h"
#include "core/input.h"
#include "render/renderer.h"
#include "render/shader_library.h"

namespace Rainy
{
    const float lt =
        (Vector3f{ 0, 0, -1 } + Vector3f{ 0, -1, 0 } + Vector3f{ 1, 0, 0 }).normalize().x;

    Rainy::Vertex3D Box::m_vertices[8] = { { { -0.5f, -0.5f, -0.5f }, { 0, 0 }, { -lt, -lt, -lt } },
                                       { { -0.5f, 0.5f, -0.5f }, { 0, 1 }, { -lt, lt, -lt } },
                                       { { 0.5f, 0.5f, -0.5f }, { 1, 1 }, { lt, lt, -lt } },
                                       { { 0.5f, -0.5f, -0.5f }, { 1, 0 }, { lt, -lt, -lt } },

                                       { { -0.5f, -0.5f, 0.5f }, { 0, 0 }, { -lt, -lt, lt } },
                                       { { -0.5f, 0.5f, 0.5f }, { 0, 1 }, { -lt, lt, lt } },
                                       { { 0.5f, 0.5f, 0.5f }, { 1, 1 }, { lt, lt, lt } },
                                       { { 0.5f, -0.5f, 0.5f }, { 1, 0 }, { lt, -lt, lt } } };

    uint32_t Box::m_indices[36] = {
        0, 3, 2, 2, 1, 0,   // front
        4, 5, 6, 6, 7, 4,   // back

        4, 0, 1, 1, 5, 4,   // left
        3, 7, 6, 6, 2, 3,   // right

        1, 2, 6, 6, 5, 1,   // top
        0, 4, 7, 7, 3, 0,   // bottom
    };

    Box::Box(Vector3f size)
    {
        m_vertexBuffer = VertexBuffer::Create(sizeof(Rainy::Vertex3D) * 8, m_vertices);
        auto elements = { BufferElement(FLOAT3, false),
                          BufferElement(FLOAT2, false),
                          BufferElement(FLOAT3, false) };
        m_vertexBuffer->SetBufferLayout({ elements, 8 });
        m_indexBuffer = IndexBuffer::Create(sizeof(m_indices), 36, m_indices);
        m_vertexArray = VertexArray::Create();
        m_vertexArray->SetVertexBuffers({ m_vertexBuffer });
        m_vertexArray->SetIndexBuffer(m_indexBuffer);
    }

    Box::~Box()
    {
        delete m_vertexArray;
        delete m_vertexBuffer;
    }

    void Box::setSize(Vector3f size) { setScale(size); }

    Vector3f Box::getSize() const { return getScale(); }

    void Box::draw() { DrawVertexArray(RenderMode::RN_TRIANGLES, m_vertexArray); }

}
