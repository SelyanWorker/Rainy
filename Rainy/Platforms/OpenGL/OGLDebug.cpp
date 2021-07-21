#include "rnpch.h"

#include "OGLDebug.h"

namespace Rainy {

	GLenum CheckError_(int line)
	{
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (errorCode)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}
			//std::cout << error << " | " << file << " (" << line << ")" << std::endl;
			RN_CORE_ERROR("{0} line: {1}", error, line);
		}
		return errorCode;
	}

	void APIENTRY OGLDebugOutput(GLenum source,
						GLenum type,
						GLuint id,
						GLenum severity,
						GLsizei length,
						const GLchar *message,
						const void *userParam)
	{
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::string srcLine = "Source: ";
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             srcLine += "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   srcLine += "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: srcLine += "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     srcLine += "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     srcLine += "Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           srcLine += "Other"; break;
		} 

		std::string typeLine = "Type: ";
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:              typeLine += "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:typeLine += "Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeLine += "Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:        typeLine += "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:        typeLine += "Performance"; break;
		case GL_DEBUG_TYPE_MARKER:             typeLine += "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:         typeLine += "Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:          typeLine += "Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:              typeLine += "Other"; break;
		} 

		std::string severityLine = "Severity: ";
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:        severityLine += "high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:      severityLine += "medium"; break;
		case GL_DEBUG_SEVERITY_LOW:         severityLine += "low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:severityLine += "notification"; break;
		} 

		RN_CORE_ERROR("OpenGL Debug message ({0}): {1} \n {2} \n {3} \n {4}", 
			id, message, srcLine, typeLine, severityLine);

	}

}