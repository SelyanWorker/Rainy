#pragma once

#include "3d/box.h"
#include "core/core.h"
#include "math/math.h"
#include "render/shader.h"

namespace Rainy
{
    class RAINY_API Light
    {
    public:
        Light(Vector3f color = { 1, 1, 1 }, float diffInt = 1, float ambInt = 1, float specInt = 1);

        virtual ~Light();

        virtual void toShader(Shader *shader);

        void drawLightBox(Shader *boxShader);

        void setPosition(Vector3f position);

        Vector3f getPosition();

    public:
        Vector3f m_color;
        float m_diffIntensity;
        float m_ambIntensity;
        float m_specIntensity;

    protected:
        Box *m_box;
    };

}
