#pragma once

#include "core/core.h"
#include "core/input.h"
#include "math/math.h"

namespace Rainy
{
    class RAINY_API Camera
    {
    public:
        virtual void onUpdate() = 0;

        virtual Matrix4f getViewMatrix() const = 0;

        virtual Matrix4f getProjectionMatrix() const = 0;

        virtual void setPosition(Vector3f position) = 0;

        virtual void setRotation(Vector3f rotation) = 0;

        virtual Vector3f getPosition() const = 0;

        virtual Vector3f getRotation() const = 0;
    };

}
