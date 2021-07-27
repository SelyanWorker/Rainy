
#include "glad/glad.h"

#include "WindowsWindow.h"
#include "core/log.h"
#include "event/window_events.h"
#include "event/key_events.h"
#include "event/mouse_event.h"

//extern "C" {
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

namespace Rainy {

	Window * Window::Create(WindowProperties prop)
	{
		return new WindowsWindow(prop);
	}

	WindowsWindow::WindowsWindow(WindowProperties& prop)
	{
		m_prop.m_name = prop.m_name;
		m_prop.m_width = prop.m_width;
		m_prop.m_height = prop.m_height;
		m_prop.m_eventFun = nullptr;
		Init();
		glfwGetWindowPos(m_window, &m_prop.m_x, &m_prop.m_y);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	uint32_t WindowsWindow::GetWidth() const
	{
		return m_prop.m_width;
	}

	uint32_t WindowsWindow::GetHeight() const
	{
		return m_prop.m_height;
	}

	void * WindowsWindow::GetNativeWindow() const
	{
		return reinterpret_cast<void*>(m_window);
	}

	std::pair<int32_t, int32_t> WindowsWindow::GetWindowsPosition() const
	{
		return { m_prop.m_x, m_prop.m_y };
	}

	void WindowsWindow::SetEventFunction(EventFunType eventFun)
	{
		m_prop.m_eventFun = eventFun;
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	void WindowsWindow::SwapBuffers()
	{
		glfwSwapBuffers(m_window);
	}

	void WindowsWindow::EnableCursor()
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void WindowsWindow::DisableCursor()
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void WindowsWindow::Init()
	{
		// assertion
		RN_ASSERT(glfwInit(), "glfwInit() error");

		//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_prop.m_width, m_prop.m_height, m_prop.m_name, 0, 0);
		glfwMakeContextCurrent(m_window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		/*GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			RN_CORE_INFO("OGL debug context created successfully");
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OGLDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
		else
		{
			RN_CORE_INFO("OGL debug context was not created");
		}*/

		RN_CORE_INFO("GPU {0} {1}", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		RN_CORE_INFO("OGL {0} | GLSL {1}", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

		GLint major,
		      minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		glfwSetWindowUserPointer(m_window, &m_prop);

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowsWindowProp * prop =
				reinterpret_cast<WindowsWindowProp*>(glfwGetWindowUserPointer(window));
			WindowCloseEvent e;
			prop->m_eventFun(e);
		});

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow * window, int width, int height)
		{
			WindowsWindowProp * prop =
				reinterpret_cast<WindowsWindowProp * >(glfwGetWindowUserPointer(window));
			prop->m_width = width;
			prop->m_height = height;
			WindowResizeEvent e(width, height);
			prop->m_eventFun(e);
		});

		glfwSetWindowPosCallback(m_window, [](GLFWwindow * window, int x, int y)
		{
			WindowsWindowProp * prop =
				reinterpret_cast<WindowsWindowProp *>(glfwGetWindowUserPointer(window));
			prop->m_x = x;
			prop->m_y = y;
			WindowResizeEvent e(x, y);
			prop->m_eventFun(e);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow * window, int Key, int scancode, int action, int mods)
		{
			WindowsWindowProp * prop =
				reinterpret_cast<WindowsWindowProp * >(glfwGetWindowUserPointer(window));
			switch (action)
			{
			case GLFW_PRESS:
            {
                auto event = KeyPressEvent(Key);
                prop->m_eventFun(event);
            }
            break;
			case GLFW_REPEAT:
            {
                auto event = KeyPressEvent(Key);
                prop->m_eventFun(event);
            }
            break;
			case GLFW_RELEASE:
            {
                auto event = KeyReleaseEvent(Key);
                prop->m_eventFun(event);
            }
            break;
			default:
				break;
			}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint)
		{
			WindowsWindowProp * prop =
				reinterpret_cast<WindowsWindowProp *>(glfwGetWindowUserPointer(window));
			auto event = CharInputEvent(codepoint);
			prop->m_eventFun(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
		{
			WindowsWindowProp * prop =
				reinterpret_cast<WindowsWindowProp * >(glfwGetWindowUserPointer(window));
			MouseMoveEvent e(xpos, ypos);
			prop->m_eventFun(e);
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			WindowsWindowProp * prop =
				reinterpret_cast<WindowsWindowProp * >(glfwGetWindowUserPointer(window));
			MouseScrollEvent e(xoffset, yoffset);
			prop->m_eventFun(e);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowsWindowProp * prop =
				reinterpret_cast<WindowsWindowProp * >(glfwGetWindowUserPointer(window));
			switch (action)
			{
			case GLFW_PRESS:
			{
                auto event = MouseKeyPressEvent(button);
			    prop->m_eventFun(event);
			}
			break;
			case GLFW_REPEAT:
            {
                auto event = MouseKeyPressEvent(button);
                prop->m_eventFun(event);
            }
            break;
			case GLFW_RELEASE:
            {
                auto event = MouseKeyReleaseEvent(button);
                prop->m_eventFun(event);
            }
            break;
			default:
				break;
			}
		});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
}
