#pragma once

#include "render/texture_2d.h"

#include <initializer_list>
#include <utility>

namespace Rainy {

	class RAINY_API FrameBuffer
	{
	public:
		static FrameBuffer* Create(std::initializer_list<Texture2D*> colorTextures);

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		virtual void SetSize(float width, float height) = 0;

		virtual std::pair<float, float> GetSize() = 0;

		virtual Texture2D* GetAttachmentColorTexture(uint32_t number) = 0;

		virtual void* GetColorData(uint32_t number) = 0;

	};

}
