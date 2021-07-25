#pragma once

#include "core/Core.h"
#include "math/Math.h"
#include "geometry/Vertex.h"
#include "Buffer.h"

namespace Rainy {

	class RAINY_API Shader
	{
	public:
		static Shader* Create(std::vector<std::string>& src);

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		virtual void SetUniform(const char* name, bool value) = 0;

		virtual void SetUniform(const char* name, uint32_t value) = 0;

		virtual void SetUniform(const char* name, int32_t value) = 0;

		virtual void SetUniform(const char* name, float value) = 0;

		virtual void SetUniform(const char* name, Vector2f vector) = 0;

		virtual void SetUniform(const char* name, Vector3f vector) = 0;

		virtual void SetUniform(const char* name, Vector4f vector) = 0;

		virtual void SetUniform(const char* name, Matrix3f matrix, bool transpose = true) = 0;

		virtual void SetUniform(const char* name, Matrix4f matrix, bool transpose = true) = 0;

		virtual Vector4f GetUniformValue(const char* name) const = 0;

		virtual void AddStorageBlock(ShaderBuffer* buffer, const char* name) = 0;

	};



}
