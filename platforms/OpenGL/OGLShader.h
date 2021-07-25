#pragma once

#include "core/Core.h"
#include "render/Shader.h"

#include <glad/glad.h>

namespace Rainy {

	class OGLShader : public Shader
	{
	public:
		OGLShader(std::vector<std::string>& src);

		~OGLShader();

		void Bind() override;

		void UnBind() override;

		void SetUniform(const char* name, bool value) override;

		void SetUniform(const char* name, uint32_t value) override;

		void SetUniform(const char* name, int32_t value) override;

		void SetUniform(const char* name, float value) override;

		void SetUniform(const char* name, Vector2f vector) override;

		void SetUniform(const char* name, Vector3f vector) override;

		void SetUniform(const char* name, Vector4f vector) override;

		void SetUniform(const char* name, Matrix3f matrix, bool transpose = true) override;

		void SetUniform(const char* name, Matrix4f matrix, bool transpose = true) override;

		Vector4f GetUniformValue(const char* name) const override;

		void AddStorageBlock(ShaderBuffer* buffer, const char* name) override;

		bool GetLinkStatus() const;

	private:
		GLint GetUniformLocation(const char* name) const;

		GLuint CreateShader(GLenum Type, const char* src, GLsizei srcLenght);

	private:
		GLuint m_index;
		GLuint m_vsIndex;
		GLuint m_fsIndex;
		GLuint m_gsIndex;
		GLuint m_tcsIndex;
		GLuint m_tesIndex;
		GLuint m_lastStorageIndex;
	};

}
