#pragma once

#include "core/core.h"
#include "texture.h"

#include <cstdint>

namespace Rainy {

	class RAINY_API Texture2D : public Texture
	{
	public:
		static Texture2D* Create();

		virtual uint32_t GetWidth() = 0;

		virtual uint32_t GetHeight() = 0;

		virtual void TextureData(uint32_t width, uint32_t height,
			uint32_t channels, TextureDataType dataType, void* data) = 0;

		virtual void TextureData(uint32_t width, uint32_t height, void* data) = 0;

		virtual void TextureData(Image* image) = 0;

		virtual void TextureSubData(uint32_t widthOffset, uint32_t heightOffset,
			uint32_t width, uint32_t height, void* data) = 0;

		virtual void* GetTextureData() = 0;

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		virtual uint32_t GetTexUnit() const = 0;

		virtual void SetTexUnit(uint32_t unit) = 0;

	protected:
		uint32_t m_width;
		uint32_t m_height;
	};


}
