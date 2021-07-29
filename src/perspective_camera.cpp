#include "camera/perspective_camera.h"
#include "application/application.h"

namespace Rainy
{
    /*Vector3f PerspectiveCamera::getMoveIncrement(Vector3f direction)
    {
        using std::sin;
        using std::cos;

        float xRotRad = ToRadian(m_rotation.x);
        float yRotRad = ToRadian(m_rotation.y);

        Vector3f result = {
            direction.z * sin(yRotRad) - direction.x * cos(yRotRad),
            direction.z * sin(xRotRad) + direction.y,
            direction.z * cos(yRotRad) + direction.x * sin(yRotRad)
        };

        return result;
    }*/

    PerspectiveCamera::PerspectiveCamera(float aspectRation,
                                         float fov,
                                         float nearPoint,
                                         float farPoint,
                                         Vector3f position,
                                         Vector3f rotation,
                                         float rotationSpeed,
                                         float moveSpeed)
      : m_position(position),
        m_rotation(rotation),
        m_rotationSpeed(rotationSpeed),
        m_moveSpeed(moveSpeed),
        m_freeze(false)
    {
        m_view = createViewMatrix(m_position, m_rotation.x, m_rotation.y);
        m_projection = createPerspectiveMatrix(aspectRation, fov, nearPoint, farPoint);
    }

    void PerspectiveCamera::onUpdate()
    {
        static std::pair<float, float> prevCursorPosition = Input::getCursorPosition();
        std::pair<float, float> cursorPosition = Input::getCursorPosition();

        if (m_freeze)
        {
            prevCursorPosition = cursorPosition;
            return;
        }

        float frameTime = Application::get()->getFrameTime();

        Vector3f prevPosition = m_position;
        Vector3f prevRotation = m_rotation;

        m_rotation.x +=
            (cursorPosition.second - prevCursorPosition.second) * m_rotationSpeed * frameTime;
        m_rotation.y -=
            (cursorPosition.first - prevCursorPosition.first) * m_rotationSpeed * frameTime;
        prevCursorPosition = cursorPosition;

        if (m_rotation.x > 89)
            m_rotation.x = 89;
        else if (m_rotation.x < -89)
            m_rotation.x = -89;

        Vector3f direction = getLookDirection(m_rotation);
        float yRotRad = ToRadian(m_rotation.y);
        float halfRad = PI / 2.f;
        Vector3f right = { std::sin(yRotRad - halfRad), 0, std::cos(yRotRad - halfRad) };
        Vector3f up = right.cross(direction);

        if (Input::isKeyPressed(RN_KEY_W))
        {
            m_position += direction * (-m_moveSpeed);
        }
        if (Input::isKeyPressed(RN_KEY_S))
        {
            m_position += direction * m_moveSpeed;
        }
        if (Input::isKeyPressed(RN_KEY_D))
        {
            m_position += right * (-m_moveSpeed);
        }
        if (Input::isKeyPressed(RN_KEY_A))
        {
            m_position += right * m_moveSpeed;
        }
        if (Input::isKeyPressed(RN_KEY_R))
        {
            direction = { 0, 0, 0 };
            m_position = direction;
            m_rotation = direction;
        }
        if (Input::isKeyPressed(RN_KEY_Q))
        {
            m_position += up * m_moveSpeed;
        }
        if (Input::isKeyPressed(RN_KEY_E))
        {
            m_position += up * (-m_moveSpeed);
        }

        if (prevPosition != m_position || prevRotation != m_rotation)
            m_view = createViewMatrix(m_position, m_rotation.x, m_rotation.y);
        // m_view = createLookAtMatrix(m_position, direction + m_position, up);
    }

    Matrix4f PerspectiveCamera::getViewMatrix() const { return m_view; }

    Matrix4f PerspectiveCamera::getProjectionMatrix() const { return m_projection; }

    void PerspectiveCamera::setPosition(Vector3f position)
    {
        if (position != m_position)
        {
            m_position = position;
            m_view = createViewMatrix(m_position, m_rotation.x, m_rotation.y);
        }
    }

    void PerspectiveCamera::setRotation(Vector3f rotation)
    {
        if (rotation != m_rotation)
        {
            m_rotation = rotation;
            m_view = createViewMatrix(m_position, m_rotation.x, m_rotation.y);
        }
    }

    Vector3f PerspectiveCamera::getPosition() const { return m_position; }

    Vector3f PerspectiveCamera::getRotation() const { return m_rotation; }

    void PerspectiveCamera::freeze() { m_freeze = true; }

    void PerspectiveCamera::unFreeze() { m_freeze = false; }

    bool PerspectiveCamera::isFrozen() const { return m_freeze; }

    void PerspectiveCamera::recreateProjectionMatrix(float aspectRation,
                                                     float fov,
                                                     float nearPoint,
                                                     float farPoint)
    {
        m_projection = createPerspectiveMatrix(aspectRation, fov, nearPoint, farPoint);
    }

}
