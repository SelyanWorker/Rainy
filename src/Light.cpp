
#include "light/Light.h"

namespace Rainy {

	const char* LIGHT_POSITION_UNIFORM_NAME = "lightPosition";
	const char* LIGHT_COLOR_UNIFORM_NAME = "lightColor";
	const char* AMB_INT_UNIFORM_NAME = "ambInt";
	const char* DIFF_INT_UNIFORM_NAME = "diffInt";
	const char* SPEC_INT_UNIFORM_NAME = "specInt";

	Light::Light(Vector3f color, float diffInt, float ambInt, float specInt)
	:	m_color(color),
		m_ambIntensity(ambInt),
		m_diffIntensity(diffInt),
		m_specIntensity(specInt)
	{
		m_box = new Box({ 10, 10, 10 });
	}

	Light::~Light()
	{
		delete m_box;
	}

	void Light::ToShader(Shader* shader)
	{
		shader->SetUniform(LIGHT_POSITION_UNIFORM_NAME, m_box->GetPosition());
		shader->SetUniform(LIGHT_COLOR_UNIFORM_NAME, m_color);
		//shader->SetUniform(AMB_INT_UNIFORM_NAME, m_ambIntensity);
		shader->SetUniform(DIFF_INT_UNIFORM_NAME, m_diffIntensity);
		//shader->SetUniform(SPEC_INT_UNIFORM_NAME, m_specIntensity);
	}

	void Light::DrawLightBox(Shader* boxShader)
	{
		boxShader->SetUniform("modelMatrix", m_box->GetModelMatrix());
		m_box->Draw();
	}

	void Light::SetPosition(Vector3f position)
	{
		m_box->SetPosition(position);
	}

	Vector3f Light::GetPosition()
	{
		return m_box->GetPosition();
	}

}
