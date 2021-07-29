#pragma once

#include "render/shader.h"

#include <unordered_map>

namespace Rainy
{
    class RAINY_API ShaderLibrary
    {
    public:
        static Shader *CreateShader(std::istream *input, std::string shaderName = "");

        static Shader *CreateShaderFromFile(std::string file);

        static Shader *GetShader(std::string name);

    private:
        static std::unordered_map<std::string, Shader *> m_shaders;
    };

}
