#pragma once

#include "core/core.h"
#include "core/log.h"

namespace Rainy
{
    enum RAINY_API ShaderDataType : int32_t
    {
        NONE = 0,
        FLOAT2 = 1,
        FLOAT3 = 2
    };

    struct RAINY_API BufferElement
    {
        BufferElement(ShaderDataType type, bool normalize = false, uint32_t divisor = 0)
          : Type(type),
            Normalize(normalize),
            Divisor(divisor)
        {
        }

        ShaderDataType Type = ShaderDataType::NONE;
        bool Normalize = false;
        uint32_t Divisor = 0;   //  rate at which generic vertex attributes advance during instanced
                                //  rendering (from OGL doc)
        uint32_t Size = 1;
        uint32_t Offset = 0;
    };

    class RAINY_API BufferLayout
    {
    public:
        BufferLayout() = default;

        BufferLayout(std::initializer_list<BufferElement> &elements, uint64_t vertexCount)
          : m_elements(elements),
            m_stride(0),
            m_vertexCount(vertexCount)
        {
            for (auto &element : m_elements)
            {
                switch (element.Type)
                {
                    case FLOAT2:
                        element.Size = 2;
                        element.Offset = m_stride;
                        m_stride += sizeof(float) * 2;
                        break;
                    case FLOAT3:
                        element.Size = 3;
                        element.Offset = m_stride;
                        m_stride += sizeof(float) * 3;
                        break;
                    case NONE:
                        RN_CORE_ERROR("No data type specified!");
                        break;
                    default:
                        RN_CORE_ERROR("From BufferLayout constructor -> default in type switch");
                }
            }
        }

        uint32_t GetStride() const { return m_stride; }
        uint64_t GetVertexCount() const { return m_vertexCount; }

        std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_elements.end(); }

    private:
        std::vector<BufferElement> m_elements;
        uint32_t m_stride = 0;
        uint64_t m_vertexCount = 0;
    };

    class RAINY_API VertexBuffer
    {
    public:
        static VertexBuffer *Create(uint32_t size, void *data);

        virtual void SetBufferLayout(BufferLayout layout) = 0;

        virtual BufferLayout const &GetBufferLayout() const = 0;

        virtual void SubData(int64_t offset, int64_t size, void *data) = 0;

        virtual void Reallocate(uint32_t size, void *data) = 0;
        // virtual void* getData(uint32_t buffer, int64_t offset, int64_t size, void* data);

        virtual void Bind() = 0;

        virtual void UnBind() = 0;
    };

    class RAINY_API IndexBuffer
    {
    public:
        static IndexBuffer *Create(uint32_t size, uint32_t count, void *data);

        virtual uint32_t GetCount() const = 0;

        virtual void SubData(int64_t offset, int64_t size, void *data) = 0;

        virtual void Reallocate(uint32_t size, void *data, uint32_t count) = 0;

        virtual void Bind() = 0;

        virtual void UnBind() = 0;
    };

    class RAINY_API ShaderBuffer
    {
    public:
        static ShaderBuffer *Create(uint32_t size, void *data);

        virtual void *GetData(uint32_t offset, uint32_t size) const = 0;

        virtual void SubData(int64_t offset, int64_t size, void *data) = 0;

        virtual void Reallocate(uint32_t size, void *data) = 0;

        virtual void Bind() = 0;

        virtual void UnBind() = 0;
    };

}
