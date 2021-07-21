#pragma once

#include "Rainy/Core/Input.h"
#include "WindowsWindow.h"

#include "Rainy/Core/Application.h"

namespace Rainy {

	class WindowsInput : public Input
	{
	protected:
		bool IsKeyPressedImpl(uint32_t KeyCode) override;
		bool IsKeyReleaseImpl(uint32_t KeyCode)  override;
		bool IsMouseKeyPressedImpl(uint32_t mouseKeyCode)  override;
		bool IsMouseKeyReleasedImpl(uint32_t mouseKeyCode)  override;
		std::pair<float, float> GetCursorPositionImpl()  override;
		//std::pair<float, float> GetMouseScrollImpl()  override;
	};

}