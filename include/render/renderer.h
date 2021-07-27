#pragma once

#include "render/vertex_array.h"

namespace Rainy {

	enum RenderMode
	{
		RN_POINTS = 0x0000,
		RN_LINES = 0x0001,
		RN_LINE_LOOP = 0x0002,
		RN_LINE_STRIP = 0x0003,
		RN_TRIANGLES = 0x0004,
		RN_TRIANGLE_STRIP = 0x0005,
		RN_TRIANGLE_FAN = 0x0006,
		RN_PATCHES = 0x000E
	};

	RAINY_API void Setup();

	RAINY_API void Clear();

	RAINY_API void DrawArray(RenderMode mode, uint32_t vertexArray, uint32_t count);

	RAINY_API void DrawElements(RenderMode mode, uint32_t vertexArray, uint32_t count);

	RAINY_API uint32_t CreateBuffer();

	RAINY_API void BufferData(uint32_t buffer, void* dataPtr, size_t dataSize);

	RAINY_API void BufferSubData(uint32_t buffer, int64_t offset, int64_t size, void* data);

	RAINY_API void IndicesBufferData(uint32_t buffer, void* dataPtr, size_t dataSize);

	RAINY_API void IndicesBufferSubData(uint32_t buffer, int64_t offset, int64_t size, void* data);

	RAINY_API uint32_t CreateVertexArray();

	RAINY_API void BufferAttach(uint32_t vertexArray, uint32_t buffer,
		uint32_t index = 0, uint32_t size = 3, uint32_t stride = 0, void* pointer = nullptr);

	RAINY_API void IndicesBufferAttach(uint32_t vertexArray, uint32_t buffer);

	RAINY_API void EnableRestartIndex(uint32_t index);

	RAINY_API void DisableRestartIndex();

	RAINY_API void EnablePolygonMode();

	RAINY_API void DisablePolygonMode();

	RAINY_API void EnableCullFace();

	RAINY_API void DisableCullFace();

	RAINY_API void SetPointSize(float size);

	RAINY_API void SetLineWidth(float width);

	RAINY_API float GetPointSize();

	RAINY_API float GetLineWidth();

	RAINY_API void DeleteBuffer(uint32_t buffer);

	RAINY_API void SetPathSize(uint32_t size);

	RAINY_API void RendererTest();

	// new
	RAINY_API void DrawVertexArray(RenderMode mode, VertexArray* array);



}
