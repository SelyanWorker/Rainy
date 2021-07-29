#pragma once

#include "core.h"
#include "key_codes.h"
#include "window.h"

namespace Rainy
{
    class RAINY_API Input
    {
    public:
        static bool IsKeyPressed(uint32_t KeyCode) { return m_input->IsKeyPressedImpl(KeyCode); }
        static bool IsKeyReleased(uint32_t KeyCode) { return m_input->IsKeyReleaseImpl(KeyCode); }
        static bool IsMouseKeyPressed(uint32_t mouseKeyCode)
        {
            return m_input->IsMouseKeyPressedImpl(mouseKeyCode);
        }
        static bool IsMouseKeyReleased(uint32_t mouseKeyCode)
        {
            return m_input->IsMouseKeyReleasedImpl(mouseKeyCode);
        }
        static std::pair<float, float> GetCursorPosition()
        {
            return m_input->GetCursorPositionImpl();
        }
        //	static std::pair<float, float> GetMouseScroll() { return m_input->GetMouseScrollImpl();
        //}

    protected:
        virtual bool IsKeyPressedImpl(uint32_t KeyCode) = 0;
        virtual bool IsKeyReleaseImpl(uint32_t KeyCode) = 0;
        virtual bool IsMouseKeyPressedImpl(uint32_t mouseKeyCode) = 0;
        virtual bool IsMouseKeyReleasedImpl(uint32_t mouseKeyCode) = 0;
        virtual std::pair<float, float> GetCursorPositionImpl() = 0;
        //	virtual std::pair<float, float> GetMouseScrollImpl() = 0;

    private:
        static Input *m_input;
    };

}
