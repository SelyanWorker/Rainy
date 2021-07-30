#pragma once

#include "geometry/vertex.h"
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

        void setSize(Vector3f size);

        Vector3f getSize() const;

        void draw();

    private:
        static Vertex3D m_vertices[8];
        static uint32_t m_indices[36];

        VertexBuffer *m_vertexBuffer;
        IndexBuffer *m_indexBuffer;
        VertexArray *m_vertexArray;
    };

}
