#pragma once

#include "camera.h"
#include "core/core.h"

namespace Rainy
{
    class RAINY_API PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera(float aspectRation,
                          float fov,
                          float nearPoint,
                          float farPoint,
                          Vector3f position = { 0, 0, 0 },
                          Vector3f rotation = { 0, 0, 0 },
                          float rotationSpeed = 1,
                          float moveSpeed = 1);

        ~PerspectiveCamera() = default;

        void onUpdate() override;

        Matrix4f getViewMatrix() const override;

        Matrix4f getProjectionMatrix() const override;

        void setPosition(Vector3f position) override;

        void setRotation(Vector3f rotation) override;

        Vector3f getPosition() const override;

        Vector3f getRotation() const override;

        void freeze();

        void unFreeze();

        bool isFrozen() const;

        void recreateProjectionMatrix(float aspectRation,
                                      float fov,
                                      float nearPoint,
                                      float farPoint);

    private:
        Vector3f getMoveIncrement(Vector3f direction);

    public:
        float m_rotationSpeed;
        float m_moveSpeed;

    private:
        Matrix4f m_view;
        Matrix4f m_projection;

        float m_near;
        float m_far;
        float m_ar;
        float m_fov;

        Vector3f m_position;
        Vector3f m_rotation;

        bool m_freeze;
    };

}
