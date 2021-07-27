#pragma once

#include "Rainy/Render/CameraController.h"

namespace Rainy {

	class RAINY_API BasicCameraController : public CameraController
	{
	public:
		BasicCameraController()
		{
			m_camera = nullptr;
		}

		~BasicCameraController() = default;

		void Update() override
		{
			
		}

	};


}