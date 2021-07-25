
#include "LandscapePlus.h"
#include "Rainy/Render/Renderer.h"

namespace ls {
	constexpr uint32_t SECTION_RESTART_INDEX = 0u - 1;
	constexpr uint8_t RAY_PARTS_COUNT = 1000;
	constexpr float SECTION_PART_SIZE = 1.f;
	constexpr uint32_t RECURSION_COUNT = 100;

	float CORRECTIVE_VALUE = 0.0f;

	Landscape::Landscape(LandscapeInitSize size, SectionSize sectionSize)
	{
		uint32_t uSize = static_cast<uint32_t>(size);
		uint32_t uSectionSize = static_cast<uint32_t>(sectionSize);

		if (size == LandscapeInitSize::_64 &&
			uSectionSize > static_cast<uint32_t>(SectionSize::_127)) uSectionSize = static_cast<uint32_t>(SectionSize::_127);
		if (size == LandscapeInitSize::_32 &&
			uSectionSize > static_cast<uint32_t>(SectionSize::_255)) uSectionSize = static_cast<uint32_t>(SectionSize::_255);
		if (size == LandscapeInitSize::_16 &&
			uSectionSize > static_cast<uint32_t>(SectionSize::_511)) uSectionSize = static_cast<uint32_t>(SectionSize::_511);
		if (size == LandscapeInitSize::_8 &&
			uSectionSize > static_cast<uint32_t>(SectionSize::_1023)) uSectionSize = static_cast<uint32_t>(SectionSize::_1023);
		if (size == LandscapeInitSize::_4 &&
			uSectionSize > static_cast<uint32_t>(SectionSize::_2047)) uSectionSize = static_cast<uint32_t>(SectionSize::_2047);
		
		m_mapWidth = uSize * 2;
		m_mapLength = uSize * 2;
		m_sectionSize = uSectionSize;
		m_sectionWorldSize = SECTION_PART_SIZE * uSectionSize;

		uint32_t val = m_sectionSize * uSize;
		uint32_t prec = 1;
		while ((val / uint32_t(std::powf(10, prec))) != 0) prec++;
		RN_ASSERT(prec <= 7, "prec > 7 in Landscape constructor");
		CORRECTIVE_VALUE = std::powf(0.1f, 7 - prec);

		m_sectionsMap.resize(m_mapWidth * m_mapLength);
		m_sectionToDraw.reserve(m_mapWidth * m_mapLength);

		Vector3f landscapeStartPoint = { -float(uSize) * m_sectionWorldSize / 2, 0, -float(uSize) * m_sectionWorldSize / 2 };

		const uint32_t verticesInSection = uSectionSize + 1;
		const uint32_t verticesRows = uSize * uSectionSize + 1;
		const uint32_t verticesColumns = uSize * uSectionSize + 1;

		for (uint32_t z = 0; z < verticesRows; z++)
		{
			for (uint32_t x = 0; x < verticesColumns; x++)
			{
				m_vertices.push_back({
					Vector3f{ x * SECTION_PART_SIZE, 0, z * SECTION_PART_SIZE} + landscapeStartPoint,
					{ 0, 0 },
					{ 0, 1, 0 }
				});
			}
		}

		uint32_t mapIndent = uSize / 2;

		for (uint32_t z = 0; z < uSize; z++)
		{
			for (uint32_t x = 0; x < uSize; x++)
			{
				Section* section = new Section();

				uint32_t mapIndex = (z + mapIndent) * m_mapWidth + x + mapIndent;
				m_sectionsMap[mapIndex] = section;
				m_sectionToDraw.push_back(mapIndex);

				for (uint16_t zz = 0; zz < verticesInSection; ++zz)
				{
					for (uint16_t xx = 0; xx < verticesInSection; ++xx)
					{
						section->m_uniqIndices.push_back(
							(z * m_sectionSize + zz) * verticesColumns +
							(x * m_sectionSize + xx)
						);
					}
				}

				std::vector<uint32_t> indices;

				for (uint16_t zz = 0; zz < m_sectionSize; ++zz)
				{
					for (uint16_t xx = 0; xx <= m_sectionSize; ++xx)
					{
						indices.push_back(section->m_uniqIndices[zz * (m_sectionSize + 1) + xx]);
						indices.push_back(section->m_uniqIndices[(zz + 1) * (m_sectionSize + 1) + xx]);
					}
					indices.push_back(SECTION_RESTART_INDEX);
				}
				section->m_indexBuffer = Rainy::IndexBuffer::Create(indices.size() * sizeof(uint32_t), indices.size(), indices.data());
			}
		}
		m_vertexBuffer = Rainy::VertexBuffer::Create(m_vertices.size() * sizeof(Vertex), m_vertices.data());
		auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false), Rainy::BufferElement(Rainy::FLOAT2, false), Rainy::BufferElement(Rainy::FLOAT3, false) };
		m_vertexBuffer->SetBufferLayout({ elements, m_vertices.size()});
		m_vertexArray = Rainy::VertexArray::Create();
		m_vertexArray->SetVertexBuffers({ m_vertexBuffer });
	}

	void Landscape::SetScale(float scale)
	{
		static_cast<Entity*>(this)->SetScale(Vector3f(scale));
	}

	float Landscape::GetScale() const
	{
		return static_cast<Entity const*>(this)->GetScale().x;
	}

	Section* Landscape::GetSection(Vector3f point)
	{
		if (!PointInMap(point.x, point.z)) return nullptr;

		Rainy::Vector2i posInMap = SectionPosInMap({ point.x - CORRECTIVE_VALUE, point.z - CORRECTIVE_VALUE });

		return m_sectionsMap[posInMap.y * m_mapWidth + posInMap.x];
	}

	/*std::vector<Section*> Landscape::GetSections(Vector3f point, float radius)
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

		std::vector<Section*> sections;
		for (uint32_t z = sectionsArea.BottomBorder; z <= sectionsArea.TopBorder; z++)
		{
			for (uint32_t x = sectionsArea.LeftBorder; x <= sectionsArea.RightBorder; x++)
			{
				Section* section = m_sectionsMap[z * m_mapWidth + x];
				if(section != nullptr) sections.push_back(section);
			}
		}
		return sections;
	}
*/

	void Landscape::AddSection(Vector3f point)
	{
		Section* sectionInPoint = GetSection(point);
		if (sectionInPoint != nullptr)
		{
			Rainy::Vector2i posInMap = SectionPosInMap({ point.x, point.z });
			m_sectionToDraw.push_back(posInMap.y * m_mapWidth + posInMap.x);
			FlatSection(sectionInPoint);
		}

		int x = (point.x + m_mapWidth * m_sectionWorldSize / 2) / m_sectionWorldSize;
		int z = (point.z + m_mapLength * m_sectionWorldSize / 2) / m_sectionWorldSize;

		static const uint32_t vericesCount = m_sectionSize + 1;
		uint32_t xStart = 0;
		uint32_t xEnd = vericesCount;
		uint32_t zStart = 0;
		uint32_t zEnd = vericesCount;

		uint32_t sectionIndex = z * m_mapWidth + x;
		if (sectionIndex >= m_sectionsMap.size()) IncreaseMapSize();

		if (x - 1 >= 0)
		{
			sectionIndex = z * m_mapWidth + x - 1;
			if (m_sectionsMap[sectionIndex] != nullptr)
			{
				xStart += 1;
			}
		}
		if (x + 1 < m_mapWidth)
		{
			sectionIndex = z * m_mapWidth + x + 1;
			if (m_sectionsMap[sectionIndex] != nullptr)
			{
				xEnd -= 1;
			}
		}
		if (z - 1 >= 0)
		{
			sectionIndex = (z - 1) * m_mapWidth + x;
			if (m_sectionsMap[sectionIndex] != nullptr)
			{
				zStart += 1;
			}
		}
		if (z + 1 < m_mapLength)
		{
			sectionIndex = (z + 1) * m_mapWidth + x;
			if (m_sectionsMap[sectionIndex] != nullptr)
			{
				zEnd -= 1;
			}
		}

		// if not neighbor then exit 
		if (xStart == 0 && zStart == 0 && xEnd == vericesCount && zEnd == vericesCount) return;

		int xOffset = std::floorf(point.x / m_sectionWorldSize) * m_sectionWorldSize;
		int zOffset = std::floorf(point.z / m_sectionWorldSize) * m_sectionWorldSize;
		Vector3f offset = { float(xOffset), 0, float(zOffset) };

		uint32_t prevSize = m_vertices.size();

		for (uint32_t z = zStart; z < zEnd; ++z)
		{
			for (uint32_t x = xStart; x < xEnd; ++x) 
			{
				m_vertices.push_back({
					Vector3f{ float(x), 0, float(z) } + offset,
					{ 0, 0 },
					{ 0, 1, 0 }
				});
			}
		}

		Section* section = new Section;
		sectionIndex = z * m_mapWidth + x;
		m_sectionsMap[sectionIndex] = section;
		m_sectionToDraw.push_back(sectionIndex);

		section->m_uniqIndices.resize(vericesCount * vericesCount);

		uint32_t i = 0;
		for (uint32_t z = zStart; z < zEnd; ++z)
		{
			for (uint32_t x = xStart; x < xEnd; ++x, ++i)
			{
				section->m_uniqIndices[z * vericesCount + x] = prevSize + i;
			}
		}

		if (xStart == 1)
		{
			Section* neighborSection = m_sectionsMap[z * m_mapWidth + x - 1];
			for (uint32_t z = 0; z < vericesCount; ++z)
			{
				section->m_uniqIndices[z * vericesCount] =
					neighborSection->m_uniqIndices[z * vericesCount + vericesCount - 1];
			}
		}

		if (xEnd == vericesCount - 1)
		{
			Section* neighborSection = m_sectionsMap[z * m_mapWidth + x + 1];
			for (uint32_t z = 0; z < vericesCount; ++z)
			{
				section->m_uniqIndices[z * vericesCount + vericesCount - 1] =
					neighborSection->m_uniqIndices[z * vericesCount];
			}
		}
		if (zStart == 1)
		{
			Section* neighborSection = m_sectionsMap[(z - 1) * m_mapWidth + x];
			for (uint32_t x = 0; x < vericesCount; ++x)
			{
				section->m_uniqIndices[x] =
					neighborSection->m_uniqIndices[(vericesCount - 1) * vericesCount + x];
			}
		}
		if (zEnd == vericesCount - 1)
		{
			Section* neighborSection = m_sectionsMap[(z + 1) * m_mapWidth + x];
			for (uint32_t x = 0; x < vericesCount; ++x)
			{
				section->m_uniqIndices[(vericesCount - 1) * vericesCount + x] =
					neighborSection->m_uniqIndices[x];
			}
		}
		std::vector<uint32_t> indices;
		for (uint16_t ii = 0; ii < m_sectionSize; ++ii)
		{
			for (uint16_t jj = 0; jj <= m_sectionSize; ++jj)
			{
				indices.push_back(section->m_uniqIndices[ii * (m_sectionSize + 1) + jj]);
				indices.push_back(section->m_uniqIndices[(ii + 1) * (m_sectionSize + 1) + jj]);
			}
			indices.push_back(SECTION_RESTART_INDEX);
		}
		section->m_indexBuffer = Rainy::IndexBuffer::Create(indices.size() * sizeof(uint32_t), indices.size(), indices.data());
		
		ReloadVertexBuffer();

		if (x == 0 || z == 0 || x == (m_mapWidth - 1) || z == (m_mapLength - 1))
			IncreaseMapSize();
	}

	void Landscape::RemoveSection(Vector3f point)
	{
		Rainy::Vector2i posInMap = SectionPosInMap({ point.x, point.z });
		if (posInMap.x >= 0 && posInMap.y >= 0)
		{
			uint32_t index = posInMap.y * m_mapWidth + posInMap.x;
			if (m_sectionsMap[index] != nullptr)
			{
				auto result = std::find(m_sectionToDraw.begin(), m_sectionToDraw.end(), index);
				if (result != m_sectionToDraw.end())
				{
					std::vector<uint32_t> copy = m_sectionToDraw;
					m_sectionToDraw.clear();
					for (auto i : copy)	if(i != index) m_sectionToDraw.push_back(i);
				}
			}
		}
	}

	void Landscape::Draw()
	{
		Rainy::EnableRestartIndex(SECTION_RESTART_INDEX);
		for (uint32_t sectionIndex : m_sectionToDraw)
		{
			m_vertexArray->SetIndexBuffer(m_sectionsMap[sectionIndex]->m_indexBuffer);
			Rainy::DrawVertexArray(Rainy::RenderMode::RN_TRIANGLE_STRIP, m_vertexArray);
		}
		Rainy::DisableRestartIndex();
	}

	bool Landscape::RayIntersection(Vector3f& dest, Vector3f rayDirection, Vector3f rayStartPoint, float rayDistance)
	{
		float scale = GetScale();
		rayStartPoint.x /= scale;
		rayStartPoint.y /= scale;
		rayStartPoint.z /= scale;

		Vector3f startPoint;
		GetPointOnBorder(rayDirection, rayStartPoint, startPoint);
		Vector3f endPoint;
		GetPointOnBorder(rayDirection * (-1.f), rayStartPoint + (rayDirection * rayDistance), endPoint);

		float partLength = (endPoint - startPoint).length() / RAY_PARTS_COUNT;

		if (StartAboveEndBelow(startPoint, endPoint))
		{
			for (uint32_t i = 0; i < RAY_PARTS_COUNT; ++i)
			{
				Vector3f startRayPartPoint = startPoint + rayDirection * (partLength * i);
				Vector3f endRayPartPoint = startPoint + rayDirection * (partLength * (i + 1));

				if (StartAboveEndBelow(startRayPartPoint, endRayPartPoint))
				{
					dest = BinarySearch(startRayPartPoint, rayDirection, 0, partLength, RECURSION_COUNT);
					Section* section = GetSection(dest);
					return  section != nullptr;
				}
			}
		}

		return false;
	}

	LandscapePart* Landscape::GetPart(Vector3f const& center, float radius)
	{
		LandscapePart* part = new LandscapePart;

		int const zVertexOffset = (m_mapLength * m_sectionSize) / 2;
		int const xVertexOffset = (m_mapWidth * m_sectionSize) / 2;

		Rainy::RectangleAreai vertexSpaceBorders
		{
			std::ceilf((xVertexOffset + center.x - radius)),
			(xVertexOffset + center.x + radius),
			std::ceilf((zVertexOffset + center.z - radius)),
			(zVertexOffset + center.z + radius)
		};

		if (vertexSpaceBorders.LeftBorder < 0) vertexSpaceBorders.LeftBorder = 0;
		if (vertexSpaceBorders.RightBorder > xVertexOffset * 2) vertexSpaceBorders.RightBorder = xVertexOffset * 2;
		if (vertexSpaceBorders.BottomBorder < 0) vertexSpaceBorders.BottomBorder = 0;
		if (vertexSpaceBorders.TopBorder > zVertexOffset * 2) vertexSpaceBorders.TopBorder = zVertexOffset * 2;

		part->Length = std::abs(vertexSpaceBorders.TopBorder - vertexSpaceBorders.BottomBorder);
		part->Width = std::abs(vertexSpaceBorders.RightBorder - vertexSpaceBorders.LeftBorder);

		part->VerticesData.reserve((part->Length + 1) * (part->Width + 1));

		int sectionSizeInt = int(m_sectionSize);

		for (int y = vertexSpaceBorders.BottomBorder; y <= vertexSpaceBorders.TopBorder; ++y)
		{
			for (int x = vertexSpaceBorders.LeftBorder; x <= vertexSpaceBorders.RightBorder; ++x)
			{
				Rainy::Vector2i sectionPosition{
					x / sectionSizeInt,
					y / sectionSizeInt
				};
				if (sectionPosition.x >= m_mapWidth) sectionPosition.x = m_mapWidth - 1;
				if (sectionPosition.y >= m_mapLength) sectionPosition.y = m_mapLength - 1;
				Section* adjacentSection = m_sectionsMap[sectionPosition.y * m_mapWidth + sectionPosition.x];
				if (adjacentSection == nullptr)
				{
					bool xOverlay = x % m_sectionSize == 0;
					bool yOverlay = y % m_sectionSize == 0;

					Rainy::Vector2i adjacentSectionPos;
					if (xOverlay)
					{
						adjacentSectionPos = { (x - 1) / sectionSizeInt, sectionPosition.y };
						adjacentSection = m_sectionsMap[adjacentSectionPos.y * m_mapWidth + adjacentSectionPos.x];
					}
					if (adjacentSection == nullptr && yOverlay)
					{
						adjacentSectionPos = { sectionPosition.x, (y - 1) / sectionSizeInt };
						adjacentSection = m_sectionsMap[adjacentSectionPos.y * m_mapWidth + adjacentSectionPos.x];
					}
					if (adjacentSection == nullptr && xOverlay && yOverlay)
					{
						adjacentSectionPos = { (x - 1) / sectionSizeInt, (y - 1) / sectionSizeInt };
						adjacentSection = m_sectionsMap[adjacentSectionPos.y * m_mapWidth + adjacentSectionPos.x];
					}

					if (adjacentSection == nullptr)
					{
						part->VerticesData.push_back({ nullptr, SECTION_RESTART_INDEX });
						continue;
					}
					else sectionPosition = adjacentSectionPos;
				}

				Rainy::Vector2i vertexPosition
				{
					x - sectionPosition.x * sectionSizeInt,
					y - sectionPosition.y * sectionSizeInt
				};

				VertexData vertexData;
				vertexData.second =
					adjacentSection->m_uniqIndices[vertexPosition.y * (m_sectionSize + 1) + vertexPosition.x];
				vertexData.first = &m_vertices[vertexData.second];

				part->VerticesData.push_back(vertexData);
			}
		}

		return part;
	}

	void Landscape::RealoadPart(LandscapePart* part)
	{
		for (auto vertexData : part->VerticesData) {
			if (vertexData.first != nullptr) {
				m_vertexBuffer->SubData(vertexData.second * sizeof(Vertex), sizeof(Vertex), 
					(void*)(m_vertices.data() + vertexData.second));
			}
		}
	}

	void Landscape::IncreaseMapSize()
	{
		uint32_t prevWidth = m_mapWidth;
		uint32_t prevLength = m_mapLength;
		m_mapWidth *= 2;
		m_mapLength *= 2;
		std::vector<Section*> prevMap = m_sectionsMap;
		m_sectionsMap.clear();
		m_sectionsMap.resize(m_mapLength * m_mapWidth, nullptr);
		m_sectionToDraw.clear();

		uint32_t zOffset = (m_mapLength - prevLength) / 2;
		uint32_t xOffset = (m_mapWidth - prevWidth) / 2;

		for (uint32_t z = 0; z < prevLength; z++)
		{
			for (uint32_t x = 0; x < prevWidth; x++)
			{
				uint32_t newIndex = (z + zOffset) * m_mapWidth + x + xOffset;
				m_sectionsMap[newIndex] = prevMap[z * prevWidth + x];
				if (prevMap[z * prevWidth + x] != nullptr)
					m_sectionToDraw.push_back(newIndex);
			}
		}
	}

	void Landscape::ReloadVertexBuffer()
	{
		delete m_vertexBuffer;
		m_vertexBuffer = VertexBuffer::Create(m_vertices.size() * sizeof(Vertex), m_vertices.data());
		auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false),
			Rainy::BufferElement(Rainy::FLOAT2, false), Rainy::BufferElement(Rainy::FLOAT3, false) };
		m_vertexBuffer->SetBufferLayout({ elements, m_vertices.size() });
		m_vertexArray->SetVertexBuffers({ m_vertexBuffer });
	}

	bool Landscape::PointInMap(float x, float z) 
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

	bool Landscape::GetPointOnBorder(Vector3f dir, Vector3f start, Vector3f& dest)
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
			if (-(halfMapLength + intersectionPoint.z) <= CORRECTIVE_VALUE)
				intersectionPoint.z = -halfMapLength;
			pointProcess();
		}

		if (InterRayAndSurface({ -halfMapWidth, 0, 0 }, { -1, 0, 0 }, start, dir,
			intersectionPoint, rayLength))
		{
			if (-(halfMapWidth + intersectionPoint.x) <= CORRECTIVE_VALUE)
				intersectionPoint.x = -halfMapWidth;
			pointProcess();
		}

		//right top
		if (InterRayAndSurface({ 0, 0, halfMapLength }, { 0, 0, 1 }, start, dir,
			intersectionPoint, rayLength))
		{
			if ((intersectionPoint.z - halfMapLength) <= CORRECTIVE_VALUE)
				intersectionPoint.z = halfMapLength;
			pointProcess();
		}

		if (InterRayAndSurface({ halfMapWidth, 0, 0 }, { 1, 0, 0 }, start, dir,
			intersectionPoint, rayLength))
		{
			if ((intersectionPoint.x - halfMapWidth) <= CORRECTIVE_VALUE)
				intersectionPoint.x = halfMapWidth;
			pointProcess();
		}

		if (minRayLength < 0)
			return false;

		return true;
	}

	bool Landscape::GetSurfacePoint(float x, float z, Vector3f& dest)
	{
		if (!PointInMap(x, z))
			return false;

		float correctX = (x - CORRECTIVE_VALUE);
		float correctZ = (z - CORRECTIVE_VALUE);

		Rainy::Vector2i posInMap = SectionPosInMap({ correctX, correctZ });

		RN_ASSERT(posInMap.x < m_mapWidth, "posInMap.x < m_mapWidth");
		RN_ASSERT(posInMap.y < m_mapLength, "posInMap.y > m_mapLength");

		RN_ASSERT(posInMap.x >= 0, "posInMap.x > 0");
		RN_ASSERT(posInMap.y >= 0, "posInMap.y > 0");

		Section* section = m_sectionsMap[posInMap.y * m_mapWidth + posInMap.x];

		if (section == nullptr)
		{
			dest = { x, GetPosition().y, z };
			return true;
		}

		Vector3f leftBottomVertexPosition{ m_vertices[section->m_uniqIndices[0]].Position };

		/*RN_ASSERT((correctX - leftBottomVertexPosition.x) > 0, "(correctX - leftBottomVertexPosition.x) < 0");
		RN_ASSERT((correctZ - leftBottomVertexPosition.z) > 0, "(correctZ - leftBottomVertexPosition.z) < 0");*/
		RN_ASSERT((leftBottomVertexPosition.x + m_sectionWorldSize) >= correctX, "(leftBottomVertexPosition.x + m_sectionWorldSize) < correctX");
		RN_ASSERT((leftBottomVertexPosition.z + m_sectionWorldSize) >= correctZ, "(leftBottomVertexPosition.z + m_sectionWorldSize) < correctZ");

		int32_t vertexXPos = (correctX - leftBottomVertexPosition.x);
		int32_t vertexZPos = (correctZ - leftBottomVertexPosition.z); 

		RN_ASSERT(vertexXPos >= 0, "vertexXPos < 0");
		RN_ASSERT(vertexZPos >= 0, "vertexZPos < 0");

		const uint32_t vertexCount = m_sectionSize + 1;

		Vertex v0 = m_vertices[
			section->m_uniqIndices[vertexZPos * vertexCount + vertexXPos]
		];
		Vertex v1 = m_vertices[
			section->m_uniqIndices[(vertexZPos + 1) * vertexCount + vertexXPos]
		];
		Vertex v2 = m_vertices[
			section->m_uniqIndices[vertexZPos * vertexCount + vertexXPos + 1]
		];

		if ((m_sectionWorldSize - (float(vertexZPos) - leftBottomVertexPosition.z)) >
			(float(vertexXPos) - leftBottomVertexPosition.x))
			v0 = m_vertices[
				section->m_uniqIndices[(vertexZPos + 1) * vertexCount + vertexXPos + 1]
			];

		Vector3f weights = BarycentricCoord({ x, 0, z }, v0.Position, v1.Position, v2.Position);

		Vector3f point_on_surface =
			v0.Position * weights.x + v1.Position * weights.y + v2.Position * weights.z;

		dest = point_on_surface;
		return true;
	}
	
	bool Landscape::PointBelow(Vector3f point)
	{
		Vector3f pointOnSurface;
		if (GetSurfacePoint(point.x, point.z, pointOnSurface))
			return point.y <= pointOnSurface.y;
		return false;
	}

	bool Landscape::StartAboveEndBelow(Vector3f start, Vector3f end) 
	{
		return !PointBelow(start) && PointBelow(end);
	}

	Vector3f Landscape::BinarySearch(Vector3f rayStartPoint, Vector3f rayDirection,
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

	void Landscape::FlatSection(Section* section)
	{
		float landscapeY = GetPosition().y;
		for (auto vertexIndex : section->m_uniqIndices)
			m_vertices[vertexIndex].Position.y = landscapeY;
	}

	Rainy::Vector2i Landscape::SectionPosInMap(Vector2f point)
	{
		float x = (point.x + m_mapWidth / 2 * m_sectionWorldSize) / m_sectionWorldSize;
		float y = (point.y + m_mapLength / 2 * m_sectionWorldSize) / m_sectionWorldSize;
		
		Rainy::Vector2i answer = { int(x), int(y) };
		RN_ASSERT(answer.x < m_mapWidth, "posInMap.x >= m_mapWidth");
		RN_ASSERT(answer.y < m_mapLength, "posInMap.y >= m_mapLength");
		return answer;
	}

	Rainy::Image* CreateHeightMapImage(Landscape* landscape)
	{
		float highestHieght = landscape->GetPosition().y;
		float lowestHeight = landscape->GetPosition().y;
		for (auto& vertex : landscape->m_vertices)
		{
			if (vertex.Position.y > highestHieght)
				highestHieght = vertex.Position.y;
			else if (vertex.Position.y < lowestHeight) lowestHeight = vertex.Position.y;
		}

		float lhDistance = highestHieght - lowestHeight;

		Rainy::RectangleAreai sectionsArea = { landscape->m_mapWidth, 0, landscape->m_mapLength, 0};
		uint32_t areaWidth = 0;
		uint32_t areaHeight = 0;
		for (uint32_t z = 0; z < landscape->m_mapLength; z++)
		{
			for (uint32_t x = 0; x < landscape->m_mapWidth; x++)
			{
				if (landscape->m_sectionsMap[z * landscape->m_mapWidth + x] != nullptr)
				{
					if (sectionsArea.LeftBorder > x) sectionsArea.LeftBorder = x;
					if (sectionsArea.BottomBorder > z) sectionsArea.BottomBorder = z;
					if (sectionsArea.RightBorder < x) sectionsArea.RightBorder = x;
					if (sectionsArea.TopBorder < z) sectionsArea.TopBorder = z;
				}
			}
		}
		areaWidth = sectionsArea.RightBorder - sectionsArea.LeftBorder + 1;
		areaHeight = sectionsArea.TopBorder - sectionsArea.BottomBorder + 1;

		uint32_t sectionSize = landscape->m_sectionSize;
		uint32_t pixelsInLine = areaWidth * sectionSize + 1;
		uint8_t* pixels = new uint8_t[pixelsInLine * (areaHeight * sectionSize + 1)];

		Rainy::Vector2i pixelPosition = { 0, 0 };
		for (uint32_t z = sectionsArea.TopBorder; z >= sectionsArea.BottomBorder; z--)
		{
			pixelPosition.y = (z - sectionsArea.BottomBorder + 1) * sectionSize;
			for (uint32_t x = sectionsArea.LeftBorder; x <= sectionsArea.RightBorder; x++)
			{
				pixelPosition.x = (x - sectionsArea.LeftBorder) * sectionSize;
				Section* section = landscape->m_sectionsMap[z * landscape->m_mapWidth + x];
				if (section != nullptr)
				{
					for (uint32_t uzz = 0; uzz <= sectionSize; uzz++)
					{
						uint32_t zz = sectionSize - uzz;
						for (uint32_t xx = 0; xx <= sectionSize; xx++)
						{
							float height = landscape->m_vertices[section->m_uniqIndices[zz * (landscape->m_sectionSize + 1) + xx]].Position.y;
							height = height / lhDistance * 255;
							pixels[(pixelPosition.y - zz) * pixelsInLine + pixelPosition.x + xx] = height;
						}
					}
				}
				else
				{
					for (uint32_t zz = 0; zz <= sectionSize; zz++)
					{
						for (uint32_t xx = 0; xx <= sectionSize; xx++)
						{
							float height = landscape->GetPosition().y;
							height = height / lhDistance * 255;
							pixels[(pixelPosition.y - zz) * pixelsInLine + pixelPosition.x + xx] = height;
						}
					}
				}
			}
		}

		/*struct Pixel { uint8_t Red; uint8_t Green; uint8_t Blue; };
		Pixel* pixels = new Pixel[landscape. * 200];
		for (uint32_t i = 0; i < 200 * 200; i++) pixels[i] = { 255, 0, 0 };*/

		Rainy::Image* image = Rainy::Image::Create(pixels, 1, 
			areaWidth * sectionSize + 1, areaHeight * sectionSize + 1);

		return image;
	}

	void LandscapeTester::PointInMap_Test(Landscape* landscape)
	{
		RN_APP_INFO("PointInMap_Test: ");
		float prevSectionWorldSize = landscape->m_sectionWorldSize;
		uint32_t prevMapWidth = landscape->m_mapWidth;
		uint32_t prevMapLength = landscape->m_mapLength;

		landscape->m_sectionWorldSize = 4096.f;
		landscape->m_mapLength = 4;
		landscape->m_mapWidth = 4;

		Rainy::RectangleAreaf borders = { 
			- ((landscape->m_mapWidth / 2) * landscape->m_sectionWorldSize),
			(landscape->m_mapWidth / 2) * landscape->m_sectionWorldSize,
			- ((landscape->m_mapLength / 2) * landscape->m_sectionWorldSize),
			(landscape->m_mapLength / 2) * landscape->m_sectionWorldSize
		}; 

		float OFFSET = 0.001f;
		std::array<std::pair<Vector2f, bool>, 8> points;
		points[0] = { { borders.LeftBorder - OFFSET, borders.BottomBorder - OFFSET	}, false }; // left bottom OUT
		points[1] = { { borders.LeftBorder + OFFSET, borders.BottomBorder + OFFSET	}, true	};	// left bottom IN 
		points[2] = { { borders.LeftBorder - OFFSET, borders.TopBorder	+ OFFSET	}, false }; // left top OUT	   
		points[3] = { { borders.LeftBorder + OFFSET, borders.TopBorder	- OFFSET	}, true	};	// left top IN
		points[4] = { { borders.RightBorder + OFFSET, borders.TopBorder + OFFSET	}, false };	// right top OUT
		points[5] = { { borders.RightBorder - OFFSET, borders.TopBorder - OFFSET	}, true };	// right top IN  
		points[6] = { { borders.RightBorder + OFFSET, borders.BottomBorder - OFFSET }, false }; // right bottom OUT
		points[7] = { { borders.RightBorder - OFFSET, borders.BottomBorder + OFFSET }, true	};	// right bottom IN
		
		for (auto& point : points)
			RN_ASSERT(point.second == landscape->PointInMap(point.first.x, point.first.y), "PointInMapTest FAILED!");
		RN_APP_INFO("LandscapeTester->PointInMap_Test: OFFSET = 0.001f Done!");

		OFFSET = 0.0001f;
		points[0] = { { borders.LeftBorder - OFFSET, borders.BottomBorder - OFFSET	}, false }; // left bottom OUT
		points[1] = { { borders.LeftBorder + OFFSET, borders.BottomBorder + OFFSET	}, true };	// left bottom IN 
		points[2] = { { borders.LeftBorder - OFFSET, borders.TopBorder + OFFSET	}, false };		// left top OUT	   
		points[3] = { { borders.LeftBorder + OFFSET, borders.TopBorder - OFFSET	}, true };		// left top IN
		points[4] = { { borders.RightBorder + OFFSET, borders.TopBorder + OFFSET	}, false };	// right top OUT
		points[5] = { { borders.RightBorder - OFFSET, borders.TopBorder - OFFSET	}, true };	// right top IN  
		points[6] = { { borders.RightBorder + OFFSET, borders.BottomBorder - OFFSET }, false }; // right bottom OUT
		points[7] = { { borders.RightBorder - OFFSET, borders.BottomBorder + OFFSET }, true };	// right bottom IN

		for (auto& point : points)
			RN_ASSERT(point.second == landscape->PointInMap(point.first.x, point.first.y), "PointInMapTest FAILED!");
		RN_APP_INFO("LandscapeTester->PointInMap_Test: OFFSET = 0.0001f Done!");

		OFFSET = 0.00001f;
		points[0] = { { borders.LeftBorder - OFFSET, borders.BottomBorder - OFFSET	}, false }; // left bottom OUT
		points[1] = { { borders.LeftBorder + OFFSET, borders.BottomBorder + OFFSET	}, true };	// left bottom IN 
		points[2] = { { borders.LeftBorder - OFFSET, borders.TopBorder + OFFSET	}, false };		// left top OUT	   
		points[3] = { { borders.LeftBorder + OFFSET, borders.TopBorder - OFFSET	}, true };		// left top IN
		points[4] = { { borders.RightBorder + OFFSET, borders.TopBorder + OFFSET	}, false };	// right top OUT
		points[5] = { { borders.RightBorder - OFFSET, borders.TopBorder - OFFSET	}, true };	// right top IN  
		points[6] = { { borders.RightBorder + OFFSET, borders.BottomBorder - OFFSET }, false }; // right bottom OUT
		points[7] = { { borders.RightBorder - OFFSET, borders.BottomBorder + OFFSET }, true };	// right bottom IN

		for (auto& point : points)
			RN_ASSERT(point.second == landscape->PointInMap(point.first.x, point.first.y), "PointInMapTest FAILED!");
		RN_APP_INFO("LandscapeTester->PointInMap_Test: OFFSET = 0.00001f Done!");

		OFFSET = 0.000001f;
		points[0] = { { borders.LeftBorder - OFFSET, borders.BottomBorder - OFFSET	}, false }; // left bottom OUT
		points[1] = { { borders.LeftBorder + OFFSET, borders.BottomBorder + OFFSET	}, true };	// left bottom IN 
		points[2] = { { borders.LeftBorder - OFFSET, borders.TopBorder + OFFSET	}, false };		// left top OUT	   
		points[3] = { { borders.LeftBorder + OFFSET, borders.TopBorder - OFFSET	}, true };		// left top IN
		points[4] = { { borders.RightBorder + OFFSET, borders.TopBorder + OFFSET	}, false };	// right top OUT
		points[5] = { { borders.RightBorder - OFFSET, borders.TopBorder - OFFSET	}, true };	// right top IN  
		points[6] = { { borders.RightBorder + OFFSET, borders.BottomBorder - OFFSET }, false }; // right bottom OUT
		points[7] = { { borders.RightBorder - OFFSET, borders.BottomBorder + OFFSET }, true };	// right bottom IN

		for (auto& point : points)
			RN_ASSERT(point.second == landscape->PointInMap(point.first.x, point.first.y), "PointInMapTest FAILED!");
		RN_APP_INFO("LandscapeTester->PointInMap_Test: OFFSET = 0.000001f Done!");

		landscape->m_sectionWorldSize = prevSectionWorldSize;
		landscape->m_mapLength = prevMapLength;
		landscape->m_mapWidth = prevMapWidth;
	}

	void LandscapeTester::SectionPosInMap_Test(Landscape* landscape)
	{
		RN_APP_INFO("SectionPosInMap_Test: ");
		float prevSectionWorldSize = landscape->m_sectionWorldSize;
		uint32_t prevMapWidth = landscape->m_mapWidth;
		uint32_t prevMapLength = landscape->m_mapLength;

		landscape->m_sectionWorldSize = 4096.f;
		landscape->m_mapLength = 4;
		landscape->m_mapWidth = 4;

		Rainy::RectangleAreaf borders = {
			-((landscape->m_mapWidth / 2) * landscape->m_sectionWorldSize),
			(landscape->m_mapWidth / 2) * landscape->m_sectionWorldSize,
			-((landscape->m_mapLength / 2) * landscape->m_sectionWorldSize),
			(landscape->m_mapLength / 2) * landscape->m_sectionWorldSize
		};
		
		float OFFSET = 0.001f;
		std::array<std::pair<Vector2f, Rainy::Vector2i>, 8> points;
		points[0] = { { borders.LeftBorder + OFFSET, borders.BottomBorder + OFFSET	}, { 0, 0 } };	// left bottom IN 
		points[1] = { { borders.LeftBorder + OFFSET, borders.TopBorder - OFFSET	}, { 0, 3 } };		// left top IN
		points[2] = { { borders.RightBorder - OFFSET, borders.TopBorder - OFFSET }, { 3, 3 } };	// right top IN
		points[3] = { { borders.RightBorder - OFFSET, borders.BottomBorder + OFFSET }, { 3, 0 } };	// right bottom IN
		points[4] = { { 0, borders.BottomBorder + OFFSET	}, { 2, 0 } };	// 0 bottom IN 
		points[5] = { { 0, borders.TopBorder - OFFSET	}, { 2, 3 } };			// 0 top IN
		points[6] = { { borders.LeftBorder + OFFSET, 0 }, { 0, 2 } };		// left 0 IN
		points[7] = { { borders.RightBorder - OFFSET, 0 }, { 3, 2 } };		// right 0 IN

		for (auto& point : points)
			RN_ASSERT(point.second == landscape->SectionPosInMap(point.first), "SectionPosInMap_Test FAILED!");
		RN_APP_INFO("LandscapeTester->SectionPosInMap_Test: OFFSET = 0.001f Done!");

		OFFSET = 0.0001f;
		points[0] = { { borders.LeftBorder + OFFSET, borders.BottomBorder + OFFSET	}, { 0, 0 } };	// left bottom IN 
		points[1] = { { borders.LeftBorder + OFFSET, borders.TopBorder - OFFSET	}, { 0, 3 } };		// left top IN
		points[2] = { { borders.RightBorder - OFFSET, borders.TopBorder - OFFSET }, { 3, 3 } };	// right top IN
		points[3] = { { borders.RightBorder - OFFSET, borders.BottomBorder + OFFSET }, { 3, 0 } };	// right bottom IN
		points[4] = { { 0, borders.BottomBorder + OFFSET	}, { 2, 0 } };	// 0 bottom IN 
		points[5] = { { 0, borders.TopBorder - OFFSET	}, { 2, 3 } };			// 0 top IN
		points[6] = { { borders.LeftBorder + OFFSET, 0 }, { 0, 2 } };		// left 0 IN
		points[7] = { { borders.RightBorder - OFFSET, 0 }, { 3, 2 } };		// right 0 IN

		for (auto& point : points)
			RN_ASSERT(point.second == landscape->SectionPosInMap(point.first), "SectionPosInMap_Test FAILED!");
		RN_APP_INFO("LandscapeTester->SectionPosInMap_Test: OFFSET = 0.0001f Done!");

		OFFSET = 0.00001f;
		points[0] = { { borders.LeftBorder + OFFSET, borders.BottomBorder + OFFSET	}, { 0, 0 } };	// left bottom IN 
		points[1] = { { borders.LeftBorder + OFFSET, borders.TopBorder - OFFSET	}, { 0, 3 } };		// left top IN
		points[2] = { { borders.RightBorder - OFFSET, borders.TopBorder - OFFSET }, { 3, 3 } };	// right top IN
		points[3] = { { borders.RightBorder - OFFSET, borders.BottomBorder + OFFSET }, { 3, 0 } };	// right bottom IN
		points[4] = { { 0, borders.BottomBorder + OFFSET	}, { 2, 0 } };	// 0 bottom IN 
		points[5] = { { 0, borders.TopBorder - OFFSET	}, { 2, 3 } };			// 0 top IN
		points[6] = { { borders.LeftBorder + OFFSET, 0 }, { 0, 2 } };		// left 0 IN
		points[7] = { { borders.RightBorder - OFFSET, 0 }, { 3, 2 } };		// right 0 IN

		for (auto& point : points)
			RN_ASSERT(point.second == landscape->SectionPosInMap(point.first), "SectionPosInMap_Test FAILED!");
		RN_APP_INFO("LandscapeTester->SectionPosInMap_Test: OFFSET = 0.00001f Done!");


		landscape->m_sectionWorldSize = prevSectionWorldSize;
		landscape->m_mapLength = prevMapLength;
		landscape->m_mapWidth = prevMapWidth;
	}

	void LandscapeTester::GetSurfacePoint_Test(Landscape* landscape)
	{
		RN_APP_INFO("GetSurfacePoint_Test: ");

		Rainy::RectangleAreaf mapBorders = {
			-((landscape->m_mapWidth / 2) * landscape->m_sectionWorldSize),
			(landscape->m_mapWidth / 2) * landscape->m_sectionWorldSize,
			-((landscape->m_mapLength / 2) * landscape->m_sectionWorldSize),
			(landscape->m_mapLength / 2) * landscape->m_sectionWorldSize
		};
		constexpr float OFFSET = 10.f;

		Rainy::RectangleAreaf borders = mapBorders;
		for (; borders.BottomBorder <= borders.TopBorder; borders.BottomBorder += CORRECTIVE_VALUE)
		{
			for (; borders.LeftBorder <= borders.RightBorder; borders.LeftBorder += CORRECTIVE_VALUE)
			{
				Vector3f result;
				RN_ASSERT(landscape->GetSurfacePoint(mapBorders.LeftBorder, mapBorders.BottomBorder, result), 
					"(All in) GetSurfacePoint_Test FAILED!");
			}
		}

		borders = {
			mapBorders.LeftBorder - OFFSET, mapBorders.LeftBorder - CORRECTIVE_VALUE,
			mapBorders.BottomBorder, mapBorders.TopBorder
		};
		for (; borders.BottomBorder <= borders.TopBorder; borders.BottomBorder += CORRECTIVE_VALUE)
		{
			for (; borders.LeftBorder <= borders.RightBorder; borders.LeftBorder += CORRECTIVE_VALUE)
			{
				Vector3f result;
				RN_ASSERT(landscape->GetSurfacePoint(mapBorders.LeftBorder, mapBorders.BottomBorder, result),
					"(All out left) GetSurfacePoint_Test FAILED!");
			}
		}

		borders = {
			mapBorders.RightBorder + CORRECTIVE_VALUE, mapBorders.RightBorder + OFFSET,
			mapBorders.BottomBorder, mapBorders.TopBorder
		};
		for (; borders.BottomBorder <= borders.TopBorder; borders.BottomBorder += CORRECTIVE_VALUE)
		{
			for (; borders.LeftBorder <= borders.RightBorder; borders.LeftBorder += CORRECTIVE_VALUE)
			{
				Vector3f result;
				RN_ASSERT(landscape->GetSurfacePoint(mapBorders.LeftBorder, mapBorders.BottomBorder, result),
					"(All out right) GetSurfacePoint_Test FAILED!");
			}
		}

		borders = {
			mapBorders.LeftBorder, mapBorders.RightBorder,
			mapBorders.BottomBorder - OFFSET, mapBorders.BottomBorder - CORRECTIVE_VALUE
		};
		for (; borders.BottomBorder <= borders.TopBorder; borders.BottomBorder += CORRECTIVE_VALUE)
		{
			for (; borders.LeftBorder <= borders.RightBorder; borders.LeftBorder += CORRECTIVE_VALUE)
			{
				Vector3f result;
				RN_ASSERT(landscape->GetSurfacePoint(mapBorders.LeftBorder, mapBorders.BottomBorder, result),
					"(All out bottom) GetSurfacePoint_Test FAILED!");
			}
		}

		borders = {
			mapBorders.LeftBorder, mapBorders.RightBorder,
			mapBorders.TopBorder + CORRECTIVE_VALUE, mapBorders.TopBorder + OFFSET
		};
		for (; borders.BottomBorder <= borders.TopBorder; borders.BottomBorder += CORRECTIVE_VALUE)
		{
			for (; borders.LeftBorder <= borders.RightBorder; borders.LeftBorder += CORRECTIVE_VALUE)
			{
				Vector3f result;
				RN_ASSERT(landscape->GetSurfacePoint(mapBorders.LeftBorder, mapBorders.BottomBorder, result),
					"(All out top) GetSurfacePoint_Test FAILED!");
			}
		}

		RN_APP_INFO("GetSurfacePoint_Test PASSED!");
	}

}