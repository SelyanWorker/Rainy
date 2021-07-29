#pragma once

#include "core/core.h"
#include "math/math.h"

namespace Rainy
{
    class RAINY_API Entity
    {
    public:
        Entity(Vector3f position = { 0, 0, 0 },
               Vector3f rotation = { 0, 0, 0 },
               Vector3f scale = { 1, 1, 1 })
          : m_position(position),
            m_rotation(rotation),
            m_scale(scale)
        {
            m_modelMatrix = createModelMatrix(m_position, m_rotation, m_scale);
        }

        ~Entity() = default;

        Matrix4f getModelMatrix() const { return m_modelMatrix; }

        Vector3f getPosition() const { return m_position; }

        Vector3f getRotation() const { return m_rotation; }

        Vector3f getScale() const { return m_scale; }

        void setPosition(Vector3f position)
        {
            m_position = position;
            m_modelMatrix = createModelMatrix(m_position, m_rotation, m_scale);
        }

        void setRotation(Vector3f rotation)
        {
            m_rotation = rotation;
            m_modelMatrix = createModelMatrix(m_position, m_rotation, m_scale);
        }

        void setScale(Vector3f scale)
        {
            m_scale = scale;
            m_modelMatrix = createModelMatrix(m_position, m_rotation, m_scale);
        }

    private:
        Vector3f m_position;
        Vector3f m_rotation;
        Vector3f m_scale;

        Matrix4f m_modelMatrix;
    };

}
