#pragma once

#include "core/Core.h"
#include "render/Texture2D.h"

#include <glad/glad.h>

#include <cstdint>

namespace Rainy{

	class OGLTexture2D : public Texture2D
	{
	public:
		OGLTexture2D();

		~OGLTexture2D();

		uint32_t GetNative() override;

		uint32_t GetWidth() override;

		uint32_t GetHeight() override;

		void TextureData(uint32_t width, uint32_t height,
			uint32_t channels, TextureDataType dataType, void* data) override;

		virtual void TextureData(uint32_t width, uint32_t height, void* data) override;

		void TextureData(Image* image) override;

		void TextureSubData(uint32_t widthOffset, uint32_t heightOffset,
			uint32_t width, uint32_t height, void* data) override;

		void* GetTextureData() override;

		void Bind() override;

		void UnBind() override;

		uint32_t GetTexUnit() const override { return m_texUnit; }

		void SetTexUnit(uint32_t unit) override;

	private:
		GLuint m_index;

		GLuint m_texUnit;

		GLenum m_internalFormat;
		GLenum m_dataFormat;
		GLenum m_dataType;
	};

}
