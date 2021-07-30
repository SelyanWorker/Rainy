#include "render/shader_library.h"
#include "core/log.h"

namespace Rainy
{
    std::unordered_map<std::string, Shader *> ShaderLibrary::m_shaders;

    Shader *ShaderLibrary::CreateShader(std::istream *input, std::string shaderName)
    {
        if (input == nullptr)
            return nullptr;

        std::vector<std::string> sources;
        std::string *currentSrc = nullptr;
        std::string buffer;

        for (std::getline(*input, buffer); !input->eof(); std::getline(*input, buffer))
        {
            if (buffer.find("#type") != std::string::npos)
            {
                sources.push_back("");
                currentSrc = &sources.back();
            }

            if (currentSrc != nullptr)
                *currentSrc += buffer + "\n";
        }
        // file process

        RN_ASSERT(currentSrc != nullptr, "ShaderLibrary.cpp 32");

        Shader *shader = Shader::Create(sources);
        if (shader != nullptr)
        {
            if (shaderName == "")
                shaderName += std::to_string(m_shaders.size());
            RN_CORE_INFO("Shader from {0} file loaded successfully.", shaderName);

            m_shaders.emplace(shaderName, shader);

            return shader;
        }

        return nullptr;
    }

    Shader *ShaderLibrary::CreateShaderFromFile(std::string fileName)
    {
        std::vector<std::string> sources;

        // file process
        std::string *currentSrc = nullptr;
        std::string buffer;

        std::ifstream file(fileName);
        if (!file.good())
            RN_CORE_ERROR("File {0} not found!", fileName.data());

        return CreateShader(&file, fileName);

        // for(std::getline(file, buffer); !file.eof(); std::getline(file, buffer))
        //{
        //	if (buffer.find("#type") != std::string::npos)
        //	{
        //		sources.push_back("");
        //		currentSrc = &sources.back();
        //	}

        //	if (currentSrc != nullptr)
        //		*currentSrc += buffer + "\n";
        //}
        ////file process
        //
        // RN_ASSERT(currentSrc != nullptr, "ShaderLibrary.cpp 32");

        // Shader* shader = Shader::Create(sources);
        // if (shader != nullptr)
        //{
        //	RN_CORE_INFO("Shader from {0} file loaded successfully.", fileName);
        //	m_shaders.emplace(fileName, shader);
        //	return shader;
        // }
        //
        // return nullptr;
    }

    Shader *ShaderLibrary::GetShader(std::string name)
    {
        auto findResult = m_shaders.find(name);
        if (findResult != m_shaders.end())
            return findResult->second;
        return nullptr;
    }

}
