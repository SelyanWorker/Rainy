#pragma once

#include "core/core.h"
#include "core/entity.h"
#include "math/math.h"
#include "render/buffer.h"
#include "render/shader.h"
#include "render/vertex_array.h"

namespace Rainy
{
    class RAINY_API Box : public Entity
    {
    public:
        Box(Vector3f size);

        ~Box();

        void SetSize(Vector3f size);

        Vector3f GetSize() const;

        void Draw();

    private:
        struct Vertex
        {
            Vector3f Position;
            Vector2f TextureCoord;
            Vector3f Normal;
        };

        static Vertex m_vertices[8];
        static uint32_t m_indices[36];

        VertexBuffer *m_vertexBuffer;
        IndexBuffer *m_indexBuffer;
        VertexArray *m_vertexArray;
    };

}
