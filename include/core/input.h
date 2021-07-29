#pragma once

#include "core.h"
#include "key_codes.h"
#include "window.h"

namespace Rainy
{
    class RAINY_API Input
    {
    public:
        static bool isKeyPressed(uint32_t KeyCode) { return m_input->isKeyPressedImpl(KeyCode); }
        static bool isKeyReleased(uint32_t KeyCode) { return m_input->isKeyReleaseImpl(KeyCode); }
        static bool isMouseKeyPressed(uint32_t mouseKeyCode)
        {
            return m_input->isMouseKeyPressedImpl(mouseKeyCode);
        }
        static bool isMouseKeyReleased(uint32_t mouseKeyCode)
        {
            return m_input->isMouseKeyReleasedImpl(mouseKeyCode);
        }
        static std::pair<float, float> getCursorPosition()
        {
            return m_input->getCursorPositionImpl();
        }
        //	static std::pair<float, float> GetMouseScroll() { return m_input->GetMouseScrollImpl();
        //}

    protected:
        virtual bool isKeyPressedImpl(uint32_t KeyCode) = 0;
        virtual bool isKeyReleaseImpl(uint32_t KeyCode) = 0;
        virtual bool isMouseKeyPressedImpl(uint32_t mouseKeyCode) = 0;
        virtual bool isMouseKeyReleasedImpl(uint32_t mouseKeyCode) = 0;
        virtual std::pair<float, float> getCursorPositionImpl() = 0;
        //	virtual std::pair<float, float> GetMouseScrollImpl() = 0;

    private:
        static Input *m_input;
    };

}
