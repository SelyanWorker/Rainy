#pragma once

#include "core/core.h"
#include "render/camera.h"

namespace Rainy {

	class RAINY_API CameraController
	{
	public:
		virtual void Update() = 0;

		Camera* GetCamera() const { return m_camera; }

		void SetCamera(Camera* cam) { if( m_camera == nullptr ) m_camera = cam; }

	protected:
		Camera* m_camera;
	};

}
