
#include "LandscapePlus.h"
#include "Rainy/Render/Renderer.h"

namespace ls {
	constexpr uint32_t SECTION_RESTART_INDEX = 0u - 1;
	constexpr uint8_t RAY_PARTS_COUNT = 1000;
	constexpr float SECTION_PART_SIZE = 1.f;
	/*bool SectionPlus::m_rememberVertices = false;
	std::vector<uint32_t> SectionPlus::m_changedVertices;*/

	SectionPlus::SectionPlus(Vector3f position, uint32_t size)
	:	m_size(size),
		m_vertexCount(m_size + 1),
		m_sectionWorldSize(m_vertexCount * SECTION_PART_SIZE)
	{
		RN_ASSERT(m_vertexCount > 1, "Error in section constructor m_vertexCount: {0}", m_vertexCount);

		m_vertices.reserve(m_vertexCount * m_vertexCount);
		for (uint32_t z = 0; z < m_vertexCount; z++)
		{
			for (uint32_t x = 0; x < m_vertexCount; x++)
			{
				m_vertices.push_back({
					Vector3f{x * SECTION_PART_SIZE, 0, z * SECTION_PART_SIZE} + position,
					{ 0, 0 }, { 0, 1, 0 } 
				});
			}
		}

		for (uint32_t z = 0; z < m_size; z++)
		{
			for (uint32_t x = 0; x < m_vertexCount; x++)
			{
				m_indices.push_back(z * m_vertexCount + x);
				m_indices.push_back((z + 1) * m_vertexCount + x);
			}
			m_indices.push_back(SECTION_RESTART_INDEX);
		}

		m_vertexBuffer = VertexBuffer::Create(m_vertices.size() * sizeof(VertexPlus), m_vertices.data());
		auto elemets = { BufferElement(Rainy::FLOAT3, false), BufferElement(Rainy::FLOAT2, false),
			BufferElement(Rainy::FLOAT3, false) };
		m_vertexBuffer->SetBufferLayout({ elemets, m_vertices.size() });
		m_indexBuffer = IndexBuffer::Create(m_indices.size() * sizeof(uint32_t), m_indices.size(), m_indices.data());
		m_vertexArray = VertexArray::Create();
		m_vertexArray->SetVertexBuffer(m_vertexBuffer);
		m_vertexArray->SetIndexBuffer(m_indexBuffer);
	}

	SectionPlus::~SectionPlus()
	{
		delete m_vertexArray;
		delete m_vertexBuffer;
		delete m_indexBuffer;
	}

	SectionPart* SectionPlus::GetPart(Vector3f center, float radius)
	{
		Rainy::RectangleAreaf centerRec{
			center.x - radius,
			center.x + radius,
			center.z - radius,
			center.z + radius
		};
		Vector3f firstVertexPos = m_vertices.begin()->Position;
		Vector3f lastVertexPos = (m_vertices.end() - 1)->Position;
		Rainy::RectangleAreaf sectionRec{
			firstVertexPos.x,
			lastVertexPos.x,
			firstVertexPos.z,
			lastVertexPos.z
		};
		Rainy::RectangleAreaf overlay;
		if (!Rainy::RectangleAreasOverlay(sectionRec, centerRec, overlay)) return nullptr;
		overlay.LeftBorder -= firstVertexPos.x;
		overlay.RightBorder -= firstVertexPos.x;
		overlay.BottomBorder -= firstVertexPos.z;
		overlay.TopBorder -= firstVertexPos.z;

		std::pair<uint32_t, uint32_t> begin
		{
			uint32_t(std::ceilf(overlay.LeftBorder / SECTION_PART_SIZE)),
			uint32_t(std::ceilf(overlay.BottomBorder / SECTION_PART_SIZE))
		};
		std::pair<uint32_t, uint32_t> end
		{
			uint32_t(std::floorf(overlay.RightBorder / SECTION_PART_SIZE)),
			uint32_t(std::floorf(overlay.TopBorder / SECTION_PART_SIZE))
		};

		if (end.first > m_size) end.first = m_size;
		if (end.second > m_size) end.second = m_size;

		if (begin.first > end.first || begin.second > end.second) return nullptr;
		
		SectionPart* part = new SectionPart(begin, end);
		part->Vertices.reserve((end.second - begin.second) * (end.first - begin.first));
		for (uint32_t z = begin.second; z <= end.second; z++)
		{
			for (uint32_t x = begin.first; x <= end.first; x++)
			{
				part->Vertices.push_back(m_vertices[z * (m_size + 1) + x]);
			}
		}
		return part;
	}

	// apply new vertex data and reload vbo part 
	void SectionPlus::ReloadPart(SectionPart* part)
	{
		RN_ASSERT(part != nullptr, "part == nullptr in ReloadPart");

		auto& begin = part->Begin;
		auto& end = part->End;
		//uint32_t partWidth = end.first - begin.first + 1;
		for (uint32_t z = begin.second; z <= end.second; z++)
		{
			for (uint32_t x = begin.first; x <= end.first; x++)
			{
				m_vertices[z * m_vertexCount + x] =
					part->Vertices[(z - begin.second) * part->Width + x - begin.first];
			}
		}
		uint32_t bytesInWidth = part->Width * sizeof(VertexPlus);
		for (uint32_t z = begin.second; z <= end.second; z++)
		{
			uint32_t firstVertex = (z * m_vertexCount + begin.first);
			m_vertexBuffer->SubData(firstVertex * sizeof(VertexPlus), bytesInWidth, m_vertices.data() + firstVertex);
		}
	}

	bool SectionPlus::GetPointHeight(float x, float z, float& height)
	{
		auto& firstVertexPos = m_vertices[0].Position;
		auto& secondVertexPos = m_vertices.back().Position;
		if (firstVertexPos.x > x || firstVertexPos.z > z || secondVertexPos.x < x || secondVertexPos.z < z)
			return false;

		// from now on, it is guaranteed that the point inside the section
		const float CORRECTIVE_VALUE = m_sectionWorldSize / 1000.f;
		
		float correctX = x - CORRECTIVE_VALUE;
		float correctZ = z - CORRECTIVE_VALUE;
		
		Vector3f leftButtomVertexPosition{ m_vertices[0].Position };

		float xOffset = (correctX - leftButtomVertexPosition.x);
		float zOffset = (correctZ - leftButtomVertexPosition.z);

		if (xOffset < 0) xOffset = 0;
		if (zOffset < 0) zOffset = 0;

		// RN_APP_TRACE("Offset x{0} z{1}, Ñorrect x{2} z{3}, LeftBUttom {4}", xOffset, zOffset, correctX, correctZ, leftButtomVertexPosition.str());
		
		/*RN_ASSERT(xOffset >= 0 && zOffset >= 0,
			"Error in GetSurfacePoint: xOffset < 0 or zOffset < 0");*/

		// vertex position in section virtual space { 0, vertexCount }
		// float vertexStep = m_sectionWorldSize / m_sectionParts;
		uint32_t vertexXPos = xOffset / SECTION_PART_SIZE;
		uint32_t vertexZPos = zOffset / SECTION_PART_SIZE;

		/*RN_ASSERT(vertexXPos < m_vertexCount && vertexZPos < m_vertexCount,
			"Error in GetSurfacePoint vertexXPos >= verticesCount or vertexZPos >= verticesCount");*/

		Vector3f vPos0 = m_vertices[vertexZPos * m_vertexCount + vertexXPos].Position;
		Vector3f vPos1 = m_vertices[(vertexZPos + 1) * m_vertexCount + vertexXPos].Position;
		Vector3f vPos2 = m_vertices[vertexZPos * m_vertexCount + vertexXPos + 1].Position;

		if ((m_sectionWorldSize - (vertexZPos * SECTION_PART_SIZE - leftButtomVertexPosition.z)) >
			(vertexXPos * SECTION_PART_SIZE - leftButtomVertexPosition.x))
			vPos0 = m_vertices[(vertexZPos + 1) * m_vertexCount + vertexXPos + 1].Position;

		Vector3f weights = BarycentricCoord({ x, 0, z }, vPos0, vPos1, vPos2);
		Vector3f pointOnLandscape =
			vPos0 * weights.x + vPos1 * weights.y + vPos2 * weights.z;

		height = pointOnLandscape.y;
		return true;
	}

	LandscapePlus::LandscapePlus(uint32_t width, uint32_t length, uint32_t sectionSize)
	:	m_mapWidth(width * 2),
		m_mapLength(length * 2),
		m_sectionSize(sectionSize),
		m_sectionWorldSize(SECTION_PART_SIZE * sectionSize)
	{
		m_sectionsMap.resize(m_mapWidth * m_mapLength, nullptr);
		m_sectionToDraw.reserve(m_mapWidth * m_mapLength);

		uint32_t mapIndentX = width / 2;
		uint32_t mapIndentZ = length / 2;

		Vector3f landscapeStartPoint = { -float(width) * m_sectionWorldSize / 2, 0, -float(length) * m_sectionWorldSize / 2 };

		for (uint32_t z = 0; z < length; z++)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				Vector3f sectionStartPosition = Vector3f(x * m_sectionWorldSize, 0, z * m_sectionWorldSize) + landscapeStartPoint;
				SectionPlus* section = new SectionPlus(sectionStartPosition, m_sectionSize);
				uint32_t mapIndex = (z + mapIndentZ) * m_mapWidth + x + mapIndentX;
				m_sectionsMap[mapIndex] = section;
				m_sectionToDraw.push_back(mapIndex);
			}
		}
	}

	void LandscapePlus::Draw()
	{
		Rainy::EnableRestartIndex(SECTION_RESTART_INDEX);
		for (uint32_t sectionIndex : m_sectionToDraw)
		{
			Rainy::DrawVertexArray(Rainy::RenderMode::RN_TRIANGLE_STRIP,
				m_sectionsMap[sectionIndex]->GetVertexArray());
		}
		Rainy::DisableRestartIndex();
	}

	bool LandscapePlus::PointInMap(float x, float z) 
	{
		Vector3f position = GetPosition();
		float topRightXOffset = position.x + (m_mapWidth / 2 * m_sectionWorldSize) - x;
		float topRightZOffset = position.z + (m_mapLength / 2 * m_sectionWorldSize) - z;

		float leftBottomXOffset = position.x - (m_mapWidth / 2 * m_sectionWorldSize) - x;
		float leftBottomZOffset = position.z - (m_mapLength / 2 * m_sectionWorldSize) - z;

		if (leftBottomXOffset > 0 || topRightXOffset < 0 ||
			leftBottomZOffset > 0 || topRightZOffset < 0) return false;

		return true;
	}

	bool LandscapePlus::GetSurfacePoint(float x, float z, Vector3f& dest)
	{
		/*if (!PointInMap(x, z))
			return false;*/

		/*const float CORRECTIVE_VALUE = m_sectionWorldSize / 1000;

		float correctX = x - CORRECTIVE_VALUE;
		float correctZ = z - CORRECTIVE_VALUE;

		uint32_t sectionXCoord = correctX / m_sectionWorldSize + m_mapWidth / 2;
		uint32_t sectionZCoord = correctZ / m_sectionWorldSize + m_mapLength / 2;
		*/

		SectionPlus* section = GetSection({ x, 0, z });
		if (section == nullptr)
		{
			if (!PointInMap(x, z))
				return false;
			dest = { x, GetPosition().y, z };
			return true;
		}
		
		float height;
		if (section->GetPointHeight(x, z, height))
		{
			dest = { x, height, z };
			return true;
		}

		return false;
	}
	
	/*bool LandscapePlus::GetSurfacePoint(float x, float z, Vector3f& dest) const
	{
		if (!PointInMap(x, z))
			return false;

		const float CORRECTIVE_VALUE = m_sectionWorldSize / 1000;

		float correctX = x - CORRECTIVE_VALUE;
		float correctZ = z - CORRECTIVE_VALUE;

		uint32_t sectionXCoord = correctX / m_sectionWorldSize + m_mapWidth / 2;
		uint32_t sectionZCoord = correctZ / m_sectionWorldSize + m_mapLength / 2;

		SectionPlus* section = m_sectionsMap[sectionZCoord * m_mapWidth + sectionXCoord];

		if (section == nullptr)
		{
			dest = { x, m_position.y, z };
			return true;
		}

		auto& vertices = section->GetVertices();
		Vector3f leftButtomVertexPosition{ vertices[0].Position };

		float xOffset = (correctX - leftButtomVertexPosition.x);
		float zOffset = (correctZ - leftButtomVertexPosition.z);

		//RN_APP_TRACE("GetSurfacePoint x{0} z{1}", x, z);
		RN_ASSERT(xOffset >= 0 && zOffset >= 0,
			"Error in GetSurfacePoint: xOffset < 0 or zOffset < 0");

		// vertex position in section virtual space { 0, vertexCount }
		uint32_t vertexXPos = xOffset / m_vertexStep;
		uint32_t vertexZPos = zOffset / m_vertexStep;

		uint32_t verticesCount = m_sectionPartsCount + 1;

		RN_ASSERT(vertexXPos < verticesCount && vertexZPos < verticesCount,
			"Error in GetSurfacePoint vertexXPos >= verticesCount or vertexZPos >= verticesCount");

		Vector3f vPos0 = vertices[vertexZPos * verticesCount + vertexXPos].Position;
		Vector3f vPos1 = vertices[(vertexZPos + 1) * verticesCount + vertexXPos].Position;
		Vector3f vPos2 = vertices[vertexZPos * verticesCount + vertexXPos + 1].Position;

		if ((float(sectionZCoord) - vertexZPos * m_vertexStep + leftButtomVertexPosition.z) >
			(m_vertexStep - (float(sectionXCoord) - vertexXPos * m_vertexStep + leftButtomVertexPosition.x)))
			vPos0 = vertices[(vertexZPos + 1) * verticesCount + vertexXPos + 1].Position;

		Vector3f weights = BarycentricCoord({ x, 0, z }, vPos0, vPos1, vPos2);

		Vector3f pointOnLandscape =
			vPos0 * weights.x + vPos1 * weights.y + vPos2 * weights.z;

		dest = pointOnLandscape;

		return true;
	}
	*/

	bool LandscapePlus::PointBelow(Vector3f point)
	{
		Vector3f pointOnSurface;
		if (GetSurfacePoint(point.x, point.z, pointOnSurface))
			return point.y <= pointOnSurface.y;
		return false;
	}

	bool LandscapePlus::StartAboveEndBelow(Vector3f start, Vector3f end) 
	{
		return !PointBelow(start) && PointBelow(end);
	}

	bool LandscapePlus::GetPointOnBorder(Vector3f dir, Vector3f start, Vector3f& dest) 
	{
		if (PointInMap(start.x, start.z))
		{
			dest = start;
			return true;
		}

		Vector3f intersectionPoint;

		float rayLength;
		float minRayLength = -1;

		auto pointProcess = [this, &dest, &rayLength, &minRayLength, &intersectionPoint]()
		{
			if (PointInMap(intersectionPoint.x, intersectionPoint.z))
			{
				if (minRayLength < 0 || minRayLength > rayLength)
				{
					minRayLength = rayLength;
					dest = intersectionPoint;
				}
			}
		};

		float halfMapWidth = m_mapWidth / 2 * m_sectionWorldSize;
		float halfMapLength = m_mapLength / 2 * m_sectionWorldSize;

		constexpr float MAGIC_VALUE = 0.0001f;

		// left bottom
		if (InterRayAndSurface({ 0, 0, -halfMapLength }, { 0, 0, -1 }, start, dir,
			intersectionPoint, rayLength))
		{
			if (-(halfMapLength + intersectionPoint.z) <= MAGIC_VALUE)
				intersectionPoint.z = -halfMapLength;
			pointProcess();
		}

		if (InterRayAndSurface({ -halfMapWidth, 0, 0 }, { -1, 0, 0 }, start, dir,
			intersectionPoint, rayLength))
		{
			if (-(halfMapWidth + intersectionPoint.x) <= MAGIC_VALUE)
				intersectionPoint.x = -halfMapWidth;
			pointProcess();
		}

		//right top
		if (InterRayAndSurface({ 0, 0, halfMapLength }, { 0, 0, 1 }, start, dir,
			intersectionPoint, rayLength))
		{
			if ((intersectionPoint.z - halfMapLength) <= MAGIC_VALUE)
				intersectionPoint.z = halfMapLength;
			pointProcess();
		}

		if (InterRayAndSurface({ halfMapWidth, 0, 0 }, { 1, 0, 0 }, start, dir,
			intersectionPoint, rayLength))
		{
			if ((intersectionPoint.x - halfMapWidth) <= MAGIC_VALUE)
				intersectionPoint.x = halfMapWidth;
			pointProcess();
		}

		if (minRayLength < 0)
			return false;

		return true;
	}

	Vector3f LandscapePlus::BinarySearch(Vector3f rayStartPoint, Vector3f rayDirection,
		float start, float end, int recursionCount)
	{
		/*RN_CORE_TRACE("RecCount: {0}", recursion_count);*/
		float half = start + (end - start) / 2;

		Vector3f rayHalfLegthPoint = rayStartPoint + rayDirection * half;

		if (recursionCount <= 0)
		{
			Vector3f surfacePoint;
			GetSurfacePoint(rayHalfLegthPoint.x, rayHalfLegthPoint.z, surfacePoint);
			return surfacePoint;
		}

		if (StartAboveEndBelow(rayStartPoint + rayDirection * start, rayHalfLegthPoint))
			return BinarySearch(rayStartPoint, rayDirection, start, half, recursionCount - 1);
		else return BinarySearch(rayStartPoint, rayDirection, half, end, recursionCount - 1);
	}

	bool LandscapePlus::RayIntersection(Vector3f& dest, Vector3f rayDirection, Vector3f rayStartPoint, float rayDistance)
	{
		float scale = GetScale();
		rayStartPoint.x /= scale;
		rayStartPoint.y /= scale;
		rayStartPoint.z /= scale;

		Vector3f startPoint;
		GetPointOnBorder(rayDirection, rayStartPoint, startPoint);
		Vector3f endPoint;
		GetPointOnBorder(rayDirection * (-1.f), rayStartPoint + (rayDirection * rayDistance), endPoint);

		/*RN_APP_TRACE("Start: {0}; End: {1};",
			startPoint.str(), endPoint.str());*/

		constexpr uint32_t RECURSION_COUNT = 100;

		float partLength = (endPoint - startPoint).length() / RAY_PARTS_COUNT;

		/*float partLength = m_sectionWorldSize / m_sectionPartsCount;
		const uint32_t PARTS_COUNT = (endPoint - startPoint).length() / partLength;*/

		/*auto hasSection = [this](Vector3f const & point)
		{
			const float CORRECTIVE_VALUE = m_sectionWorldSize / 1000;
			//const float CORRECTIVE_VALUE = 0.001f;

			float correctX = point.x - CORRECTIVE_VALUE;
			float correctZ = point.z - CORRECTIVE_VALUE;

			uint32_t sectionXCoord = correctX / m_sectionWorldSize + m_mapWidth / 2;
			uint32_t sectionZCoord = correctZ / m_sectionWorldSize + m_mapLength / 2;

			return m_sectionsMap[sectionZCoord * m_mapWidth + sectionXCoord] != nullptr;
		};*/

		if (StartAboveEndBelow(startPoint, endPoint))
		{
			for (uint32_t i = 0; i < RAY_PARTS_COUNT; ++i)
			{
				Vector3f start_ray_part_point = startPoint + rayDirection * (partLength * i);
				Vector3f end_ray_part_point = startPoint + rayDirection * (partLength * (i + 1));

				if (StartAboveEndBelow(start_ray_part_point, end_ray_part_point))
				{
					dest = BinarySearch(start_ray_part_point, rayDirection, 0, partLength, RECURSION_COUNT);
					SectionPlus* section = GetSection(dest);
					return  section != nullptr;
				}
			}
		}

		return false;
	}

	SectionPlus* LandscapePlus::GetSection(Vector3f point)
	{
		if (!PointInMap(point.x, point.z)) return nullptr;
		
		const float CORRECTIVE_VALUE = 0.0001;

		float correctX = point.x - CORRECTIVE_VALUE;
		float correctZ = point.z - CORRECTIVE_VALUE;

		uint32_t sectionXCoord = correctX / m_sectionWorldSize + m_mapWidth / 2;
		uint32_t sectionZCoord = correctZ / m_sectionWorldSize + m_mapLength / 2;

		return m_sectionsMap[sectionZCoord * m_mapWidth + sectionXCoord];
	}

	std::vector<SectionPlus*> LandscapePlus::GetSections(Vector3f point, float radius)
	{
		using std::clamp;
		using std::ceilf;
		using std::floorf;

		float xIndent = m_mapWidth / 2 * m_sectionWorldSize;
		float zIndent = m_mapLength / 2 * m_sectionWorldSize;
		Rainy::RectangleAreau sectionsArea = {
			(clamp(point.x - radius + xIndent, 0.f, xIndent * 2) / m_sectionWorldSize),
			(clamp(point.x + radius + xIndent, 0.f, xIndent * 2) / m_sectionWorldSize),
			(clamp(point.z - radius + zIndent, 0.f, zIndent * 2) / m_sectionWorldSize),
			(clamp(point.z + radius + zIndent, 0.f, zIndent * 2) / m_sectionWorldSize)
		};
		if (sectionsArea.RightBorder == m_mapWidth) sectionsArea.RightBorder--;
		if (sectionsArea.TopBorder == m_mapLength) sectionsArea.TopBorder--;

		std::vector<SectionPlus*> sections;
		for (uint32_t z = sectionsArea.BottomBorder; z <= sectionsArea.TopBorder; z++)
		{
			for (uint32_t x = sectionsArea.LeftBorder; x <= sectionsArea.RightBorder; x++)
			{
				SectionPlus* section = m_sectionsMap[z * m_mapWidth + x];
				if(section != nullptr) sections.push_back(section);
			}
		}
		return sections;
	}

	void LandscapePlus::SetScale(float scale)
	{
		static_cast<Entity*>(this)->SetScale(Vector3f(scale));
	}

	float LandscapePlus::GetScale() const
	{
		return static_cast<Entity const*>(this)->GetScale().x;
	}

	void LandscapePlus::TestFun()
	{
		RN_APP_INFO("Landscape TestFun start");

		/*
		[15:47:56] AppLogger: Dir ( -0.023680, 0.999695, 0.006969 ), Start ( 236.797409, -9986.953125, -69.693756 )
		[15:47:56] AppLogger: Result ( -1.000000, 52.217773, 0.294319 )

		dir {-0.00438325806 0.999804437 0.0192863848 }
		start {43.8325806 -9988.04395 -192.863846 }

		RayDir ( -0.584413, -0.430696, 0.687722 ) CamPos : ( 2.928782, 0.798290, 0.576211 )
		*/
	/*	Vector3f point;
		bool result = RayIntersection(point, { -0.584413, -0.430696, 0.687722 }, { 2.928782, 0.798290, 0.576211 }, 10000.f);
*/
		RN_APP_INFO("Landscape TestFun end");
	}
}