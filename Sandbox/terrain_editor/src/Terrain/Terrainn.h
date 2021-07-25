#pragma once

#include "core/Core.h"
#include "render/Buffer.h"
#include "render/VertexArray.h"
#include "render/Shader.h"
#include "render/ShaderLibrary.h"
#include "render/Texture2D.h"
#include "utils/Image.h"

#include "core/Entity.h"

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

	enum class TerrainInitSize : uint32_t
	{
		_2 = 2, _4 = 4, _8 = 8, _16 = 16, _32 = 32, _64 = 64, _128 = 128
	};

	constexpr TerrainInitSize MAX_INIT_SIZE = TerrainInitSize::_128;

	struct TerrainSection
	{
		Vector2f Position; // left buttom vertex position
		Vector2f TexturePosition; // left buttom vertex texture position
	};

	class TerrainArea
	{
	public:
		explicit TerrainArea(Vector2i firstIndex)
		:	FirstIndex(firstIndex)
		{}

		TerrainArea* GetCopy() const
		{
			TerrainArea* copy = new TerrainArea(FirstIndex);
			copy->Map = Map;
			copy->Positions = Positions;
			copy->Normals = Normals;
			copy->Width = Width;
			copy->Height = Height;
			return copy;
		}

	public:
		const Vector2i FirstIndex;

		std::vector<uint32_t> Map;
		static constexpr uint32_t INCORRECT_INDEX = uint32_t(0) - 1;

		std::vector<Vector3f> Positions;
		std::vector<Vector3f> Normals;
		uint32_t Width;
		uint32_t Height;
	};

	class Terrain : public Rainy::Entity
	{
		friend class TerrainTester;

	public:
		Terrain(TerrainInitSize columns, TerrainInitSize rows,
			Rainy::Image* heightMap = nullptr, Rainy::Image* normalsMap = nullptr, float heightMod = 1.f);

		~Terrain();

		void Draw(Rainy::Shader* terrainShader);

		//void SetShader(Rainy::Shader* shader) { m_shader = shader; }

		void SetScale(float scale) { static_cast<Entity*>(this)->SetScale(scale); }

		float GetScale() const { return static_cast<const Entity*>(this)->GetScale().x; }

		//Rainy::Shader* GetShader() { return m_shader; }

		std::vector<float>& GetHeights() { return m_heights; }

		bool RayIntersection(Vector3f& dest, Vector3f rayDirection, Vector3f rayStartPoint, float rayDistance) const;

		void AddSection(Vector2f point);

		void RemoveSection(Vector2f point);

		TerrainArea* GetArea(Vector2f point, float radius);

		void ApplyArea(TerrainArea* area);

		void ApplyAreaParts(TerrainArea* area, std::vector<Rainy::AABB2Di> parts);

		//void ApplyTool(BasicTool* tool);

		Rainy::Image* GetHeightMapImage() const;

		Rainy::Image* GetNormalsMapImage() const;

		//Rainy::Image* GetNormalMapImage() const;

		void Flat();

	private:
		bool PointOnTerrain(float x, float z) const;

		bool GetPointOnBorder(Vector3f lookAtVector, Vector3f rayStartPoint, Vector3f& dest) const;

		bool GetSurfacePoint(float x, float z, Vector3f& dest) const;

		bool PointBelow(Vector3f point) const;

		bool StartAboveEndBelow(Vector3f start, Vector3f end) const;

		Vector3f BinarySearch(Vector3f rayStartPoint, Vector3f ray,	float start, float end, int recursionCount) const;

		TerrainSection const* GetSection(Vector2f point) const;

		TerrainSection* GetSection(Vector2f point);

		Rainy::AABB2Di GetRealMapBorders() const;

	private:
		std::vector<float> m_heights;
		std::vector<Vector3f> m_normals;
		std::vector<TerrainSection*> m_sectionsMap;
		std::vector<TerrainSection> m_sectionsToDraw;

		Texture2D* m_heightMapTexture;
		Texture2D* m_normalMapTexture;

		uint32_t m_partsInSection;

		VertexBuffer* m_vertexBuffer;
		VertexBuffer* m_sectionBuffer;
		IndexBuffer* m_indexBuffer;
		VertexArray* m_vertexArray;

		float m_maxHeight;
		float m_minHeight;
	};

	class TerrainTester
	{
	public:
		static bool PointInMap_Test(Terrain* terrain);

		static bool GetSurfacePoint_Test_AllPointsOnTerrain(Terrain* terrain);

		static bool GetSurfacePoint_Test_BigAssertionTest(Terrain* terrain);

		/*static bool GetSection_Test_AllOnTerrain(Terrain* terrain, TerrainChunks terrainInit);

		static bool GetSection_Test_BigAssertionTest(Terrain* terrain, TerrainChunks terrainInit);*/

		static void PrintNotNullHeights(Terrain* terrain);

	};

}



