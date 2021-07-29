#pragma once

#include "camera.h"

// orthographic - ve.y strong word

namespace Rainy
{
    class RAINY_API OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left,
                           float right,
                           float bottom,
                           float top,
                           float nearPoint,
                           float farPoint,
                           Vector3f position = { 0, 0, 0 },
                           Vector3f rotation = { 0, 0, 0 });

        ~OrthographicCamera() = default;

        void onUpdate() override;

        Matrix4f getViewMatrix() const override;

        Matrix4f getProjectionMatrix() const override;

        void setPosition(Vector3f position) override;

        void setRotation(Vector3f rotation) override;

        Vector3f getPosition() const override;

        Vector3f getRotation() const override;

    private:
        Vector3f m_position;
        Vector3f m_rotation;

        Matrix4f m_orthographic;
        Matrix4f m_view;
    };

}
