#pragma once

#include "core/core.h"
#include "render/frame_buffer.h"

#include "OGLTexture2D.h"

#include <cstdint>
#include <initializer_list>
#include <utility>
#include <vector>

namespace Rainy
{
    class OGLFrameBuffer : public FrameBuffer
    {
    public:
        OGLFrameBuffer(std::initializer_list<Texture2D *> colorTextures);

        ~OGLFrameBuffer();

        void Bind() override;

        void UnBind() override;

        void SetSize(float width, float height) override;

        std::pair<float, float> GetSize() override;

        Texture2D *GetAttachmentColorTexture(uint32_t number) override;

        void *GetColorData(uint32_t number) override;

    private:
        bool m_binded;

        GLsizei m_width;
        GLsizei m_height;

        GLuint m_index;
        // std::vector<GLuint> m_textures; // 0 - depth texture, 1 to MAX_ATTACH - color textures
        std::vector<OGLTexture2D *> m_colorTextures;
        GLuint m_depthTexture;
        GLint *m_viewportData;
    };

}
