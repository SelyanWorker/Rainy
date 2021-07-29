
#include "light/light.h"

namespace Rainy
{
    const char *LIGHT_POSITION_UNIFORM_NAME = "lightPosition";
    const char *LIGHT_COLOR_UNIFORM_NAME = "lightColor";
    const char *AMB_INT_UNIFORM_NAME = "ambInt";
    const char *DIFF_INT_UNIFORM_NAME = "diffInt";
    const char *SPEC_INT_UNIFORM_NAME = "specInt";

    Light::Light(Vector3f color, float diffInt, float ambInt, float specInt)
      : m_color(color),
        m_ambIntensity(ambInt),
        m_diffIntensity(diffInt),
        m_specIntensity(specInt)
    {
        m_box = new Box({ 10, 10, 10 });
    }

    Light::~Light() { delete m_box; }

    void Light::toShader(Shader *shader)
    {
        shader->SetUniform(LIGHT_POSITION_UNIFORM_NAME, m_box->getPosition());
        shader->SetUniform(LIGHT_COLOR_UNIFORM_NAME, m_color);
        // shader->SetUniform(AMB_INT_UNIFORM_NAME, m_ambIntensity);
        shader->SetUniform(DIFF_INT_UNIFORM_NAME, m_diffIntensity);
        // shader->SetUniform(SPEC_INT_UNIFORM_NAME, m_specIntensity);
    }

    void Light::drawLightBox(Shader *boxShader)
    {
        boxShader->SetUniform("modelMatrix", m_box->getModelMatrix());
        m_box->draw();
    }

    void Light::setPosition(Vector3f position) { m_box->setPosition(position); }

    Vector3f Light::getPosition() { return m_box->getPosition(); }

}
