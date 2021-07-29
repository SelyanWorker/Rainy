#pragma once

#include "core/core.h"

namespace Rainy
{
    enum class ImageTypes
    {
        RN_PNG,
        RN_JPEG
    };

    class RAINY_API Image
    {
    public:
        static Image *Create(char const *file_name);

        static Image *Create(uint8_t const *pixels,
                             uint32_t channels,
                             uint32_t width,
                             uint32_t height);

        virtual uint32_t GetWidth() const = 0;

        virtual uint32_t GetHeight() const = 0;

        virtual uint32_t GetChannels() const = 0;

        virtual uint8_t *GetData() const = 0;

        virtual ImageTypes GetType() const = 0;

        virtual bool SaveImage(char const *file_name, ImageTypes Type) = 0;

        virtual void Rescale(size_t width, size_t height) = 0;
    };

}
