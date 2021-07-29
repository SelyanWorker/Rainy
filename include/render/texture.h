#pragma once
#include "core/core.h"
#include "utils/image.h"

namespace Rainy
{
    enum RAINY_API TextureDataType
    {
        UNSIGNED_BYTE,
        FLOAT
    };

    class RAINY_API Texture
    {
    public:
        virtual void TextureData(uint32_t width,
                                 uint32_t height,
                                 uint32_t channels,
                                 TextureDataType dataType,
                                 void *data) = 0;

        virtual void TextureData(uint32_t width, uint32_t height, void *data) = 0;

        virtual void TextureData(Image *image) = 0;

        virtual void TextureSubData(uint32_t widthOffset,
                                    uint32_t heightOffset,
                                    uint32_t width,
                                    uint32_t height,
                                    void *data) = 0;

        virtual void *GetTextureData() = 0;

        virtual void Bind() = 0;

        virtual void UnBind() = 0;

        virtual uint32_t GetTexUnit() const = 0;

        virtual uint32_t GetNative() = 0;

        virtual void SetTexUnit(uint32_t unit) = 0;
    };

}
