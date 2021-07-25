#pragma once

#include <vector>
#include "Rainy/Core/Core.h"
#include "Rainy/Core/Math.h"
#include "Rainy/Core/Entity.h"
#include "Rainy/Render/Buffer.h"
#include "Rainy/Render/VertexArray.h"

using Rainy::VertexBuffer;
using Rainy::BufferElement;
using Rainy::IndexBuffer;
using Rainy::VertexArray;
using Rainy::Vector3f;
using Rainy::Vector2f;

namespace ls {
	
	struct VertexPlus
	{
		Vector3f Position;
		Vector2f TexCoord;
		Vector3f Normal;
	};

	// rectangle section part
	class SectionPart
	{
		friend class SectionPlus;
	public:
		SectionPart(std::pair<uint32_t, uint32_t> begin, std::pair<uint32_t, uint32_t> end)
		:	Begin(begin), 
			End(end),
			Width(end.first - begin.first + 1),
			Length(end.second - begin.second + 1)
		{}
				
	public:
		std::vector<VertexPlus> Vertices;
		const uint32_t Width;
		const uint32_t Length;

	private:
		std::pair<uint32_t, uint32_t> Begin{ 0, 0 };
		std::pair<uint32_t, uint32_t> End{ 0, 0 };
	};

	class SectionPlus
	{
	public:
		// position - position left bottom vertex in world
		// stepCount = vertex count + 1
		SectionPlus(Vector3f position, uint32_t size);

		~SectionPlus();

		VertexArray* GetVertexArray() const { return m_vertexArray; }

		std::vector<VertexPlus>& GetVertices() { return m_vertices; }

		// return section part containing vertices around center in radius 
		SectionPart* GetPart(Vector3f center, float radius);

		// apply new vertex data and reload vbo part 
		void ReloadPart(SectionPart* part);

		bool GetPointHeight(float x, float z, float& height);

	private:
		uint32_t m_size;
		uint32_t m_vertexCount;
		uint32_t m_sectionWorldSize;
		std::vector<VertexPlus> m_vertices;
		std::vector<uint32_t> m_indices;

		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;
		VertexArray* m_vertexArray;

		/*static bool m_rememberVertices;
		static std::vector<uint32_t> m_changedVertices;*/
	};

	class LandscapePlus : public Rainy::Entity
	{
	public:
		LandscapePlus(uint32_t width, uint32_t length, uint32_t sectionSize);

		~LandscapePlus() = default;

		void Draw();

		bool RayIntersection(Vector3f & dest, Vector3f rayDirection, Vector3f rayStartPoint, float rayDistance);

		SectionPlus* GetSection(Vector3f point); // return section by point in world
		
		std::vector<SectionPlus*> GetSections(Vector3f point, float radius); // return section by point with radius

		void SetScale(float scale);

		float GetScale() const;

		void TestFun();

	private:
		bool GetSurfacePoint(float x, float z, Vector3f& dest);

		Vector3f BinarySearch(Vector3f rayStartPoint, Vector3f ray,
			float start, float end, int recursion_count);

		bool PointInMap(float x, float z) ;

		bool GetPointOnBorder(Vector3f lookAtVector, Vector3f rayStartPoint, Vector3f& dest);

		bool PointBelow(Vector3f point) ;

		bool StartAboveEndBelow(Vector3f start, Vector3f end);

	private:
		uint32_t m_sectionSize;
		float m_sectionWorldSize;

		uint32_t m_mapWidth;
		uint32_t m_mapLength;
		std::vector<SectionPlus*> m_sectionsMap;	//virtual section map size = { m_mapWidth, infinity, m_mapLength }
		std::vector<uint32_t> m_sectionToDraw;		//store section map indices
	};

}