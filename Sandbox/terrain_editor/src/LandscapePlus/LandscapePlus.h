#pragma once

#include <vector>
#include "Rainy/Core/Core.h"
#include "Rainy/Core/Math.h"
#include "Rainy/Core/Entity.h"
#include "Rainy/Render/Buffer.h"
#include "Rainy/Render/VertexArray.h"
#include "Rainy/Utils/Image.h"

using Rainy::VertexBuffer;
using Rainy::BufferElement;
using Rainy::IndexBuffer;
using Rainy::VertexArray;
using Rainy::Vector3f;
using Rainy::Vector2f;

namespace ls {
	
	using Point2Di = std::pair<int32_t, int32_t>;
	using Point2Df = std::pair<float, float>;

	struct Vertex
	{
		Vector3f Position;
		Vector2f TexCoord;
		Vector3f Normal;
	};

	using VertexData = std::pair<Vertex*, uint32_t>;
	struct LandscapePart
	{
		uint32_t Width{ 0 };
		uint32_t Length{ 0 };

		std::vector<VertexData> VerticesData;
	};

	enum class SectionSize : uint32_t
	{
		_1 = 1, _3 = 3, _7 = 7, _15 = 15, _31 = 31, _63 = 63, 
		_127 = 127, _255 = 255, _511 = 511, _1023 = 1023, _2047 = 2047, _4095 = 4095
	};

	enum class LandscapeInitSize : uint32_t
	{
		_2 = 2, _4 = 4, _8 = 8, _16 = 16, _32 = 32, _64 = 64
	};

	class Section
	{
	public:
		std::vector<uint32_t> m_uniqIndices;
		Rainy::IndexBuffer* m_indexBuffer;
	};

	class Landscape : public Rainy::Entity
	{
		friend Rainy::Image* CreateHeightMapImage(Landscape* landscape);
		friend class LandscapeTester;
	public:
		Landscape(LandscapeInitSize size, SectionSize sectionSize);

		~Landscape() = default;

		void SetScale(float scale);

		float GetScale() const;

		Section* GetSection(Vector3f point); // return section by point in world
		
		void AddSection(Vector3f point);
		
		void RemoveSection(Vector3f point);

		//std::vector<Section*> GetSections(Vector3f point, float radius); // return section by point with radius

		void Draw();

		bool RayIntersection(Vector3f & dest, Vector3f rayDirection, Vector3f rayStartPoint, float rayDistance);
		
		LandscapePart* GetPart(Vector3f const& center, float radius);

		void RealoadPart(LandscapePart* part);
		
	private:
		void IncreaseMapSize();

		void ReloadVertexBuffer();

		bool PointInMap(float x, float z);

		bool GetPointOnBorder(Vector3f lookAtVector, Vector3f rayStartPoint, Vector3f& dest);

		bool GetSurfacePoint(float x, float z, Vector3f& dest);

		bool PointBelow(Vector3f point) ;

		bool StartAboveEndBelow(Vector3f start, Vector3f end);

		Vector3f BinarySearch(Vector3f rayStartPoint, Vector3f ray,
			float start, float end, int recursion_count);

		void FlatSection(Section* section);

		inline Rainy::Vector2i SectionPosInMap(Vector2f point);

	private:
		uint32_t m_sectionSize;
		float m_sectionWorldSize;

		uint32_t m_mapWidth;
		uint32_t m_mapLength;
		std::vector<Vertex> m_vertices;
		std::vector<Section*> m_sectionsMap;		//virtual section map size = { m_mapWidth, infinity, m_mapLength }
		std::vector<uint32_t> m_sectionToDraw;		//store section map indices

		Rainy::VertexBuffer* m_vertexBuffer;
		Rainy::VertexArray* m_vertexArray;
	};

	Rainy::Image* CreateHeightMapImage(Landscape* landscape);

	class LandscapeTester 
	{
	public:
		static void RunAll(Landscape* landscape)
		{
			if (landscape != nullptr)
			{
				PointInMap_Test(landscape);
			}
		}

		static void PointInMap_Test(Landscape* landscape);

		static void SectionPosInMap_Test(Landscape* landscape);

		static void GetSurfacePoint_Test(Landscape* landscape);

	};

}