#pragma once

#include "Rainy/Core/Core.h"
#include "Rainy/Render/Buffer.h"
#include "Rainy/Render/VertexArray.h"
#include "Rainy/Render/Shader.h"
#include "Rainy/Render/Texture2D.h"
#include "Rainy/Utils/Image.h"

#include "Rainy/Core/Entity.h"

namespace te
{
	using Rainy::Vector2f;
	using Rainy::Vector3f;
	using Rainy::Vector2i;
	using Rainy::VertexBuffer;
	using Rainy::IndexBuffer;
	using Rainy::VertexArray;
	using Rainy::Texture2D;
	using Rainy::Shader;

	enum class TerrainChunks : uint32_t
	{
		_2 = 2, _4 = 4, _8 = 8, _16 = 16, _32 = 32
	};
	
	constexpr TerrainChunks MAX_CHUNKS = TerrainChunks::_32;
	
	struct TerrainSection
	{
		Vector2f Position; // left buttom vertex position
		Vector2f TexturePosition; // left buttom vertex texture position
	};

	class TerrainChunk
	{
	public:
		TerrainChunk(Vector2f posOffset, Vector2f texOffset);

		~TerrainChunk();

		void Fill();

		TerrainSection* GetSection(Vector2f point);

		TerrainSection* GetSection(Vector2f point) const;

		VertexBuffer* GetBuffer() const { return m_buffer; }

		uint32_t GetSectionsCount() const { return m_sectionsCount; }

		void StupidAddSection(Vector2f point);

		void RemoveSection(Vector2f point);

	private:
		bool PointOnChunk(Vector2f point) const;

		void RecreateBuffer();

	private:
		std::vector<TerrainSection*> m_sections;
		VertexBuffer* m_buffer;
		
		uint32_t m_sectionsCount;

		Vector2f m_posOffset;
		Vector2f m_texOffset;
	};

	class TerrainArea
	{
	public:
		explicit TerrainArea(Vector2i firstIndex)
		:	FirstIndex(firstIndex)
		{}

	public:
		const Vector2i FirstIndex;

		std::vector<uint32_t> Map;
		static constexpr uint32_t INCORECT_INDEX = uint32_t(0) - 1;

		std::vector<Vector3f> Positions;
		std::vector<Vector3f> Normals;
		uint32_t Width;
		uint32_t Height;
	};

	class Terrain : public Rainy::Entity
	{
		friend class TerrainTester;
	public:
		Terrain(TerrainChunks initSize);
		
		~Terrain();

		void Draw();

		void SetShader(Rainy::Shader* shader) { m_shader = shader; }

		void SetScale(float scale) { static_cast<Entity*>(this)->SetScale(scale); }

		float GetScale() const { return static_cast<const Entity*>(this)->GetScale().x; }

		Rainy::Shader* GetShader() { return m_shader; }

		std::vector<uint8_t>& GetHeights() { return m_heights; }

		bool RayIntersection(Vector3f& dest, Vector3f rayDirection, Vector3f rayStartPoint, float rayDistance) const;

		void AddSection(Vector2f point);

		void RemoveSection(Vector2f point);

		TerrainArea* GetArea(Vector2f point, float radius);
		
		void ApplyChanges(TerrainArea* area);

		//void ApplyTool(BasicTool* tool);

		Rainy::Image* GetHeightMapImage() const;

	private:
		bool PointOnTerrain(float x, float z) const;

		bool GetPointOnBorder(Vector3f lookAtVector, Vector3f rayStartPoint, Vector3f& dest) const;

		bool GetSurfacePoint(float x, float z, Vector3f& dest) const;

		bool PointBelow(Vector3f point) const;

		bool StartAboveEndBelow(Vector3f start, Vector3f end) const;

		Vector3f BinarySearch(Vector3f rayStartPoint, Vector3f ray,	float start, float end, int recursionCount) const;

		TerrainChunk* GetChunk(Vector2f point);
		
		TerrainChunk* GetChunk(Vector2f point) const;

		TerrainSection* GetSection(Vector2f point);

		TerrainSection* GetSection(Vector2f point) const;

	private:
		std::vector<uint8_t> m_heights;
		std::vector<Rainy::Vector3<uint8_t>> m_normals;
		std::vector<TerrainChunk*> m_chunksMap;
		std::vector<uint32_t> m_chunksToDraw;

		Texture2D* m_heightMapTexture;
		Texture2D* m_normalMapTexture;

		uint32_t m_partsInSection;

		VertexBuffer* m_vertexBuffer;
		VertexBuffer* m_sectionBuffer;
		IndexBuffer* m_indexBuffer;
		VertexArray* m_vertexArray;
		Shader* m_shader;
	};

	class TerrainTester
	{
	public:
		static bool PointInMap_Test(Terrain* terrain);

		static bool GetSurfacePoint_Test_AllPointsOnTerrain(Terrain* terrain);

		static bool GetSurfacePoint_Test_BigAssertionTest(Terrain* terrain);

		static bool GetSection_Test_AllOnTerrain(Terrain* terrain, TerrainChunks terrainInit);

		static bool GetSection_Test_BigAssertionTest(Terrain* terrain, TerrainChunks terrainInit);

		static void PrintNotNullHeights(Terrain* terrain);

	};

}


