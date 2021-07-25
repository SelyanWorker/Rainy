#pragma once

#include "core/Core.h"
#include "math/Math.h"
#include "3d/Box.h"
#include "render/Shader.h"

namespace Rainy {

	class RAINY_API Light
	{
	public:
		Light(Vector3f color = { 1, 1, 1 }, float diffInt = 1, float ambInt = 1, float specInt = 1);

		virtual ~Light();

		virtual void ToShader(Shader* shader);

		void DrawLightBox(Shader* boxShader);

		void SetPosition(Vector3f position);

		Vector3f GetPosition();

	public:
		Vector3f m_color;
		float m_diffIntensity;
		float m_ambIntensity;
		float m_specIntensity;

	protected:
		Box* m_box;

	};

}
