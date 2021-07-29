
#include "camera/orthographic_camera.h"
#include "core/input.h"

namespace Rainy
{
    OrthographicCamera::OrthographicCamera(float left,
                                           float right,
                                           float bottom,
                                           float top,
                                           float nearPoint,
                                           float farPoint,
                                           Vector3f position,
                                           Vector3f rotation)
      : m_position(position),
        m_rotation(rotation),
        m_orthographic(createOrthographicMatrix(left, right, bottom, top, nearPoint, farPoint)),
        m_view(createViewMatrix(m_position, rotation.x, rotation.y))
    {
    }

    void OrthographicCamera::onUpdate()
    {
        Vector3f prevPosition = m_position;
        Vector3f prevRotation = m_rotation;

        if (Input::isKeyPressed(RN_KEY_W)) {}
        if (Input::isKeyPressed(RN_KEY_S)) {}
        if (Input::isKeyPressed(RN_KEY_D)) {}
        if (Input::isKeyPressed(RN_KEY_A)) {}

        if (prevPosition != m_position || prevRotation != m_rotation)
            m_view = createViewMatrix(m_position, m_rotation.x, m_rotation.y);
    }

    Matrix4f OrthographicCamera::getViewMatrix() const { return m_view; }

    Matrix4f OrthographicCamera::getProjectionMatrix() const { return m_orthographic; }

    void OrthographicCamera::setPosition(Vector3f position)
    {
        if (position != m_position)
        {
            m_position = position;
            m_view = createViewMatrix(m_position, m_rotation.x, m_rotation.y);
        }
    }

    void OrthographicCamera::setRotation(Vector3f rotation)
    {
        if (rotation != m_rotation)
        {
            m_rotation = rotation;
            m_view = createViewMatrix(m_position, m_rotation.x, m_rotation.y);
        }
    }

    Vector3f OrthographicCamera::getPosition() const { return m_position; }

    Vector3f OrthographicCamera::getRotation() const { return m_rotation; }

}
