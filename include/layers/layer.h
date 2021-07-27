#pragma once

#include "core/core.h"
#include "event/event.h"

namespace Rainy {

	class RAINY_API Layer
	{
	public:
		virtual void OnEvent(Event& e) = 0;

		virtual void OnUpdate() = 0;

		virtual void OnImGuiRender() = 0;
	};



}
