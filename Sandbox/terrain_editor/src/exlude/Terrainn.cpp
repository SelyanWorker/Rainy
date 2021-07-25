
#include "Terrainn.h"
#include "Rainy/Render/Renderer.h"
#include "Rainy/Core/Time.h"

namespace te {
	constexpr uint32_t MAX_INIT_SIZE_VAL = static_cast<uint32_t>(TerrainInitSize::_256);

	constexpr uint32_t TERRAIN_RES = 8192 + 1;

	constexpr uint32_t PARTS_SECTION_SIZE = 32;

	constexpr uint32_t SECTION_TERRAIN_SIZE = MAX_INIT_SIZE_VAL;

	constexpr float WORLD_PART_SIZE = 1.f;
	constexpr float WORLD_SECTION_SIZE = PARTS_SECTION_SIZE * WORLD_PART_SIZE;

	constexpr float WORLD_TERRAIN_SIZE = SECTION_TERRAIN_SIZE * WORLD_SECTION_SIZE;
	constexpr float WORLD_HALF_TERRAIN_SIZE = WORLD_TERRAIN_SIZE / 2.f;

	constexpr uint32_t RAY_PARTS_COUNT = 1000;
	constexpr uint32_t RECURSION_COUNT = 100;

	constexpr float CORRECTIVE_VALUE = 0.001f;

	constexpr float TEX_SECTION_SIZE = 1.f / SECTION_TERRAIN_SIZE;

	Vector2i ConvertToCoord(Vector2f point, int32_t offset, float den)
	{
		float correctX = (point.x - CORRECTIVE_VALUE);
		float correctY = (point.y - CORRECTIVE_VALUE);

		int32_t vertexXPosi = int32_t(correctX / den);
		int32_t vertexYPosi = int32_t(correctY / den);
		vertexXPosi += offset;
		vertexYPosi += offset;

		return { vertexXPosi, vertexYPosi };
	}

	// *** Terrain ***
	// public methods

	Terrain::Terrain(TerrainInitSize initSize)
	{
		m_partsInSection = PARTS_SECTION_SIZE;
		int32_t quadTerrainRes = TERRAIN_RES * TERRAIN_RES;
		m_heights.resize(quadTerrainRes, 0);
		m_normals.resize(quadTerrainRes, Rainy::Vector3<uint8_t>{ 0, 0, 255 });
		m_sectionsMap.resize(SECTION_TERRAIN_SIZE * SECTION_TERRAIN_SIZE, nullptr);
		m_sectionsToDraw.reserve(m_sectionsMap.size());

		float initSizeVal = float(static_cast<uint32_t>(initSize));
		float fhalfInitSize = initSizeVal / 2;
		float posOffset = - WORLD_SECTION_SIZE * fhalfInitSize;
		float texOffset = 0.5f - TEX_SECTION_SIZE * fhalfInitSize;
		uint32_t sectionMapOffset = SECTION_TERRAIN_SIZE / 2 - fhalfInitSize;
		for (float y = 0; y < initSizeVal; y++)
		{
			for (float x = 0; x < initSizeVal; x++)
			{
				TerrainSection* section = new TerrainSection();
				section->Position = Vector2f{ x * WORLD_SECTION_SIZE, y * WORLD_SECTION_SIZE } + Vector2f{ posOffset };
				section->TexturePosition = Vector2f{ x * TEX_SECTION_SIZE, y * TEX_SECTION_SIZE } + Vector2f{ texOffset };

				m_sectionsMap[(y + sectionMapOffset) * SECTION_TERRAIN_SIZE + x + sectionMapOffset] = section;
				m_sectionsToDraw.push_back(*section);
			}
		}

		struct Vertex { Vector2f Position, TextureCoord; };
		std::array<Vertex, 4> vertices = {
			Vertex{ Vector2f{ 0.f, 0.f }, Vector2f{ 0.f, 0.f } },
			Vertex{ Vector2f{ 0.f, WORLD_SECTION_SIZE }, Vector2f{ 0.f, TEX_SECTION_SIZE } },
			Vertex{ Vector2f{ WORLD_SECTION_SIZE, WORLD_SECTION_SIZE }, Vector2f{ TEX_SECTION_SIZE, TEX_SECTION_SIZE } },
			Vertex{ Vector2f{ WORLD_SECTION_SIZE, 0.f }, Vector2f{ TEX_SECTION_SIZE, 0.f } }
		};
		
		std::array<uint32_t, 4> indices = { 0, 1, 2, 3 }; 
		//std::array<uint32_t, 4> indices = { 0, 3, 2, 1 };

		m_vertexBuffer = Rainy::VertexBuffer::Create(sizeof(Vertex) * vertices.size(), vertices.data());
		auto elemets = { Rainy::BufferElement(Rainy::FLOAT2, false), Rainy::BufferElement(Rainy::FLOAT2, false) };
		m_vertexBuffer->SetBufferLayout({ elemets,  vertices.size() });
		
		m_sectionBuffer = Rainy::VertexBuffer::Create(sizeof(TerrainSection) * m_sectionsMap.size(), nullptr);
		m_sectionBuffer->SubData(0, sizeof(TerrainSection) * m_sectionsToDraw.size(), m_sectionsToDraw.data());
		elemets = { Rainy::BufferElement(Rainy::FLOAT2, false, 1), Rainy::BufferElement(Rainy::FLOAT2, false, 1) };
		m_sectionBuffer->SetBufferLayout({ elemets,  m_sectionsToDraw.size() });

		m_indexBuffer = Rainy::IndexBuffer::Create(sizeof(uint32_t) * indices.size(), indices.size(), indices.data());
		
		m_vertexArray = Rainy::VertexArray::Create();
		m_vertexArray->SetIndexBuffer(m_indexBuffer);
		m_vertexArray->SetVertexBuffers({ m_vertexBuffer, m_sectionBuffer });
		m_vertexArray->SetInstanceCount(m_sectionsToDraw.size());
		
		Rainy::Image* image = Rainy::Image::Create("res/textures/hm.jpg");
		m_heightMapTexture = Texture2D::Create();
		//m_heightMapTexture->TextureData(image);
		m_heightMapTexture->TextureData(TERRAIN_RES, TERRAIN_RES, 1, Rainy::TextureDataType::UNSIGNED_BYTE, (void*)m_heights.data());
		m_heightMapTexture->SetTexUnit(0);
		delete image;
		 
		image = Rainy::Image::Create("res/textures/nm.jpg");
		m_normalMapTexture = Texture2D::Create();
		//m_normalMapTexture->TextureData(image);
		m_normalMapTexture->TextureData(TERRAIN_RES, TERRAIN_RES, 3, Rainy::TextureDataType::UNSIGNED_BYTE, (void*)m_normals.data());
		m_normalMapTexture->SetTexUnit(m_heightMapTexture->GetTexUnit() + 1);
		delete image;
		
	}

	Terrain::~Terrain()
	{
		delete m_vertexArray;
		delete m_indexBuffer;
		delete m_vertexBuffer;
	}

	void Terrain::Draw()
	{
		m_heightMapTexture->Bind();
		m_normalMapTexture->Bind();
		m_shader->Bind();
		
		m_shader->SetUniform("modelMatrix", GetModelMatrix());
		m_shader->SetUniform("maxTessLevel", float(m_partsInSection));
		m_shader->SetUniform("heightMap", int(m_heightMapTexture->GetTexUnit()));
		m_shader->SetUniform("normalMap", int(m_normalMapTexture->GetTexUnit()));
		m_shader->SetUniform("detailLevel", 25000.f);
		m_shader->SetUniform("heightFactor", 255.f);

		Rainy::DrawVertexArray(Rainy::RN_PATCHES, m_vertexArray);
	
		m_shader->UnBind();
		m_normalMapTexture->UnBind();
		m_heightMapTexture->UnBind();
	}

	bool Terrain::RayIntersection(Vector3f& dest, Vector3f rayDirection, Vector3f rayStartPoint, float rayDistance) const
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
					bool hasSection = GetSection({ dest.x, dest.z }) != nullptr;
					dest.Mul(scale);
					return hasSection;
				}
			}
		}

		return false;
	}

	void Terrain::AddSection(Vector2f point)
	{
		point.Mul(1.f / GetScale());
		if (GetSection(point) != nullptr) return;

		std::array<Vector2f, 4> adjacentsPoints = {
			Vector2f{ point.x, point.y - WORLD_SECTION_SIZE },
			Vector2f{ point.x, point.y + WORLD_SECTION_SIZE },
			Vector2f{ point.x - WORLD_SECTION_SIZE, point.y },
			Vector2f{ point.x + WORLD_SECTION_SIZE, point.y }
		};

		bool hasBottom = GetSection(adjacentsPoints[0]) == nullptr;
		bool hasTop = GetSection(adjacentsPoints[1]) == nullptr;
		bool hasLeft = GetSection(adjacentsPoints[2]) == nullptr;
		bool hasRight = GetSection(adjacentsPoints[3]) == nullptr;

		if (GetSection(adjacentsPoints[0]) == nullptr && 
			GetSection(adjacentsPoints[1]) == nullptr &&
			GetSection(adjacentsPoints[2]) == nullptr &&
			GetSection(adjacentsPoints[3]) == nullptr) return;

		uint32_t prevToDrawCount = m_sectionsToDraw.size();

		Vector2i newSectionCoord = ConvertToCoord(point + Vector2f{ WORLD_HALF_TERRAIN_SIZE }, 0, WORLD_SECTION_SIZE);
		TerrainSection* newSection = new TerrainSection();
		newSection->Position = { newSectionCoord.x * WORLD_SECTION_SIZE - WORLD_HALF_TERRAIN_SIZE, 
			newSectionCoord.y * WORLD_SECTION_SIZE - WORLD_HALF_TERRAIN_SIZE };
		newSection->TexturePosition = { newSectionCoord.x * TEX_SECTION_SIZE, newSectionCoord.y * TEX_SECTION_SIZE };
		m_sectionsMap[newSectionCoord.y * SECTION_TERRAIN_SIZE + newSectionCoord.x] = newSection;
		m_sectionsToDraw.push_back(*newSection);
		m_vertexArray->SetInstanceCount(m_sectionsToDraw.size());

		m_sectionBuffer->SubData(sizeof(TerrainSection) * prevToDrawCount, sizeof(TerrainSection), newSection);
	}

	void Terrain::RemoveSection(Vector2f point)
	{
		point.Mul(1.f / GetScale());
		TerrainSection* section = GetSection(point);
		if (section == nullptr) return;
		delete section;
		Vector2i sectionCoord = ConvertToCoord(point + Vector2f{ WORLD_HALF_TERRAIN_SIZE }, 0, WORLD_SECTION_SIZE);
		m_sectionsMap[sectionCoord.y * SECTION_TERRAIN_SIZE + sectionCoord.x] = nullptr;
		
		m_sectionsToDraw.clear();
		for (auto sectionPtr : m_sectionsMap) if (sectionPtr != nullptr) m_sectionsToDraw.push_back(*sectionPtr);

		m_sectionBuffer->Reallocate(sizeof(TerrainSection) * m_sectionsMap.size(), nullptr);
		m_sectionBuffer->SubData(0, sizeof(TerrainSection) * m_sectionsToDraw.size(), m_sectionsToDraw.data());
	}

	TerrainArea* Terrain::GetArea(Vector2f point, float radius)
	{
		Rainy::RectangleAreaf recArea = { 
			point.x - radius + WORLD_HALF_TERRAIN_SIZE, point.x + radius + WORLD_HALF_TERRAIN_SIZE,
			point.y - radius + WORLD_HALF_TERRAIN_SIZE, point.y + radius + WORLD_HALF_TERRAIN_SIZE
		};
		if (recArea.LeftBorder < 0) recArea.LeftBorder = 0;
		if (recArea.RightBorder > WORLD_TERRAIN_SIZE) recArea.RightBorder = WORLD_TERRAIN_SIZE;
		if (recArea.BottomBorder < 0) recArea.BottomBorder = 0;
		if (recArea.TopBorder > WORLD_TERRAIN_SIZE) recArea.TopBorder = WORLD_TERRAIN_SIZE;

		Vector2i firstCoord = { int(recArea.LeftBorder / WORLD_PART_SIZE), int(recArea.BottomBorder / WORLD_PART_SIZE) };
		Vector2i lastCoord = { int(recArea.RightBorder / WORLD_PART_SIZE), int(recArea.TopBorder / WORLD_PART_SIZE) };
		
		TerrainArea* terrainArea = new TerrainArea({ firstCoord.x, firstCoord.y });
		terrainArea->Width = lastCoord.x - firstCoord.x + 1;
		terrainArea->Height = lastCoord.y - firstCoord.y + 1;
		terrainArea->Positions.reserve(terrainArea->Height * terrainArea->Width);
		terrainArea->Normals.reserve(terrainArea->Height * terrainArea->Width);

		for (int32_t y = firstCoord.y; y <= lastCoord.y; y++)
		{
			for (int32_t x = firstCoord.x; x <= lastCoord.x; x++)
			{
				Vector2f currPoint2d = { x * WORLD_PART_SIZE - WORLD_HALF_TERRAIN_SIZE, y * WORLD_PART_SIZE - WORLD_HALF_TERRAIN_SIZE };
				TerrainSection* section = GetSection(currPoint2d);
				if (section == nullptr)
				{
					terrainArea->Positions.push_back({});
					terrainArea->Normals.push_back({});
					terrainArea->Map.push_back(TerrainArea::INCORRECT_INDEX);
				}
				else
				{
					Vector3f position = { currPoint2d.x, float(m_heights[y * TERRAIN_RES + x]), currPoint2d.y };
					auto& const n = m_normals[y * TERRAIN_RES + x];
					Vector3f normal = Vector3f{ float(n.x), float(n.y), float(n.z) } / 255.f;
					terrainArea->Positions.push_back(position);
					terrainArea->Normals.push_back(normal);
					terrainArea->Map.push_back(terrainArea->Positions.size() - 1);
				}
			}
		}

		RN_ASSERT((terrainArea->FirstIndex.x + terrainArea->Width - 1) <= TERRAIN_RES, "GetArea: (terrainArea->FirstIndex.x + terrainArea->Width - 1) > TERRAIN_RES");
		RN_ASSERT((terrainArea->FirstIndex.y + terrainArea->Height - 1) <= TERRAIN_RES, "GetArea: (terrainArea->FirstIndex.y + terrainArea->Width - 1) > TERRAIN_RES");

		return terrainArea;
	}

	void Terrain::ApplyChanges(TerrainArea* area)
	{
		RN_ASSERT(area != nullptr, "ApplyChanges: area == nullptr");

		Rainy::TimeStep time0 = Rainy::TimeStep::GetTime();
		Vector2i firstIndex = area->FirstIndex;
		for (int32_t y = 0; y < area->Height; y++)
		{
			for (int32_t x = 0; x < area->Width; x++)
			{
				uint32_t indexFromMap = area->Map[y * area->Width + x];
				if (indexFromMap == TerrainArea::INCORRECT_INDEX) continue;
				uint32_t terrIndex = (firstIndex.y + y) * TERRAIN_RES + firstIndex.x + x;
				RN_ASSERT(terrIndex <= (TERRAIN_RES * TERRAIN_RES), "ApplyChanges: terrIndex > (TERRAIN_RES * TERRAIN_RES)");
				m_heights[terrIndex] = area->Positions[indexFromMap].y;
				Vector3f n = area->Normals[indexFromMap] * 255.f;
				m_normals[terrIndex] = { uint8_t(n.x), uint8_t(n.y), uint8_t(n.z) };
			}
		}

		for (uint32_t y = firstIndex.y; y <= (firstIndex.y + area->Height); y++)
		{
			m_heightMapTexture->TextureSubData(firstIndex.x, y, area->Width, 1, m_heights.data() + (y * TERRAIN_RES + firstIndex.x));
			m_normalMapTexture->TextureSubData(firstIndex.x, y, area->Width, 1, m_normals.data() + (y * TERRAIN_RES + firstIndex.x));
		}
	}

	Rainy::Image* Terrain::GetHeightMapImage() const
	{
		Rainy::Image* heightMap = Rainy::Image::Create(m_heights.data(), 1, TERRAIN_RES, TERRAIN_RES);
		return heightMap;
	}

	// private methods

	bool Terrain::PointOnTerrain(float x, float z) const 
	{
		Vector3f position = GetPosition();
		float topRightXOffset = position.x + (WORLD_HALF_TERRAIN_SIZE) - x;
		float topRightZOffset = position.z + (WORLD_HALF_TERRAIN_SIZE) - z;

		float leftBottomXOffset = position.x - (WORLD_HALF_TERRAIN_SIZE) - x;
		float leftBottomZOffset = position.z - (WORLD_HALF_TERRAIN_SIZE) - z;

		if (leftBottomXOffset > 0 || topRightXOffset < 0 ||
			leftBottomZOffset > 0 || topRightZOffset < 0) return false;

		return true;
	}

	bool Terrain::GetPointOnBorder(Vector3f dir, Vector3f start, Vector3f& dest)  const
	{
		if (PointOnTerrain(start.x, start.z))
		{
			dest = start;
			return true;
		}

		Vector3f intersectionPoint;

		float rayLength;
		float minRayLength = -1;

		auto pointProcess = [this, &dest, &rayLength, &minRayLength, &intersectionPoint]()
		{
			if (PointOnTerrain(intersectionPoint.x, intersectionPoint.z))
			{
				if (minRayLength < 0 || minRayLength > rayLength)
				{
					minRayLength = rayLength;
					dest = intersectionPoint;
				}
			}
		};

		constexpr float MAGIC_VALUE = 0.0001f;

		// left bottom
		if (InterRayAndSurface({ 0, 0, -WORLD_HALF_TERRAIN_SIZE }, { 0, 0, -1 }, start, dir,
			intersectionPoint, rayLength))
		{
			if (-(WORLD_HALF_TERRAIN_SIZE + intersectionPoint.z) <= CORRECTIVE_VALUE)
				intersectionPoint.z = -WORLD_HALF_TERRAIN_SIZE;
			pointProcess();
		}

		if (InterRayAndSurface({ -WORLD_HALF_TERRAIN_SIZE, 0, 0 }, { -1, 0, 0 }, start, dir,
			intersectionPoint, rayLength))
		{
			if (-(WORLD_HALF_TERRAIN_SIZE + intersectionPoint.x) <= CORRECTIVE_VALUE)
				intersectionPoint.x = -WORLD_HALF_TERRAIN_SIZE;
			pointProcess();
		}

		//right top
		if (InterRayAndSurface({ 0, 0, WORLD_HALF_TERRAIN_SIZE }, { 0, 0, 1 }, start, dir,
			intersectionPoint, rayLength))
		{
			if ((intersectionPoint.z - WORLD_HALF_TERRAIN_SIZE) <= CORRECTIVE_VALUE)
				intersectionPoint.z = WORLD_HALF_TERRAIN_SIZE;
			pointProcess();
		}

		if (InterRayAndSurface({ WORLD_HALF_TERRAIN_SIZE, 0, 0 }, { 1, 0, 0 }, start, dir,
			intersectionPoint, rayLength))
		{
			if ((intersectionPoint.x - WORLD_HALF_TERRAIN_SIZE) <= CORRECTIVE_VALUE)
				intersectionPoint.x = WORLD_HALF_TERRAIN_SIZE;
			pointProcess();
		}

		if (minRayLength < 0)
			return false;

		return true;
	}

	bool Terrain::GetSurfacePoint(float x, float z, Vector3f& dest) const
	{
		if (!PointOnTerrain(x, z)) return false;

		Vector2f pointWithIndent = { x + WORLD_HALF_TERRAIN_SIZE, z + WORLD_HALF_TERRAIN_SIZE };

		uint32_t halfHeightMapRes = (TERRAIN_RES - 1) / 2;
		Vector2i heightCoord = ConvertToCoord(pointWithIndent, 0, WORLD_PART_SIZE);

		RN_ASSERT(heightCoord.x >= 0, "vertexXPos < 0");
		RN_ASSERT(heightCoord.y >= 0, "vertexZPos < 0");

		RN_ASSERT(heightCoord.x < (TERRAIN_RES - 1), "vertexXPosi > HEIGHT_MAP_RES");
		RN_ASSERT(heightCoord.y < (TERRAIN_RES - 1), "vertexZPosi > HEIGHT_MAP_RES");

		Vector3f v0 = { 0, float(m_heights[heightCoord.y * TERRAIN_RES + heightCoord.x]), 0 };
		Vector3f v1 = { 0, float(m_heights[(heightCoord.y + 1) * TERRAIN_RES + heightCoord.x]), WORLD_PART_SIZE };
		Vector3f v2 = { WORLD_PART_SIZE, float(m_heights[heightCoord.y * TERRAIN_RES + (heightCoord.x + 1)]), 0 };

		float vertexXPosf = float(heightCoord.x) * WORLD_PART_SIZE;
		float vertexZPosf = float(heightCoord.y) * WORLD_PART_SIZE;

		Vector2f localPosition = pointWithIndent - Vector2f{ vertexXPosf, vertexZPosf };

		if (localPosition.x > (WORLD_PART_SIZE - (localPosition.y)))
			v0 = { WORLD_PART_SIZE, float(m_heights[(heightCoord.y + 1) * TERRAIN_RES + (heightCoord.x + 1)]), WORLD_PART_SIZE };

		Vector3f weights = BarycentricCoord(localPosition, v0, v1, v2);

		Vector3f v0w = v0 * weights.x;
		Vector3f v1w = v1 * weights.y;
		Vector3f v2w = v2 * weights.z;

		Vector3f pointOnSurface = v0w + v1w + v2w + Vector3f{ x, 0, z };

		dest = pointOnSurface;

		return true;
	}

	bool Terrain::PointBelow(Vector3f point) const
	{
		Vector3f pointOnSurface;
		if (GetSurfacePoint(point.x, point.z, pointOnSurface))
			return point.y <= pointOnSurface.y;
		return false;
	}

	bool Terrain::StartAboveEndBelow(Vector3f start, Vector3f end) const
	{
		return !PointBelow(start) && PointBelow(end);
	}

	Vector3f Terrain::BinarySearch(Vector3f rayStartPoint, Vector3f rayDirection,
		float start, float end, int recursionCount) const
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

	TerrainSection* Terrain::GetSection(Vector2f point)
	{
		if (!PointOnTerrain(point.x, point.y)) return nullptr;

		Vector2i sectionCoord = ConvertToCoord(point + Vector2f{ WORLD_HALF_TERRAIN_SIZE }, 0, WORLD_SECTION_SIZE);

		RN_ASSERT(sectionCoord.x < SECTION_TERRAIN_SIZE, "sectionCoord.x >= SECTION_TERRAIN_SIZE");
		RN_ASSERT(sectionCoord.y < SECTION_TERRAIN_SIZE, "sectionCoord.y >= SECTION_TERRAIN_SIZE");

		return m_sectionsMap[sectionCoord.y * SECTION_TERRAIN_SIZE + sectionCoord.x];
	}

	TerrainSection* Terrain::GetSection(Vector2f point) const
	{
		if (!PointOnTerrain(point.x, point.y)) return nullptr;

		Vector2i sectionCoord = ConvertToCoord(point + Vector2f{ WORLD_HALF_TERRAIN_SIZE }, 0, WORLD_SECTION_SIZE);

		RN_ASSERT(sectionCoord.x < SECTION_TERRAIN_SIZE, "sectionCoord.x >= SECTION_TERRAIN_SIZE");
		RN_ASSERT(sectionCoord.y < SECTION_TERRAIN_SIZE, "sectionCoord.y >= SECTION_TERRAIN_SIZE");

		return m_sectionsMap[sectionCoord.y * SECTION_TERRAIN_SIZE + sectionCoord.x];
	}

	bool TerrainTester::PointInMap_Test(Terrain* terrain)
	{
		return true;
	}

	bool TerrainTester::GetSurfacePoint_Test_AllPointsOnTerrain(Terrain* terrain)
	{
		std::vector<Vector3f> allPoints;
		allPoints.reserve(TERRAIN_RES * TERRAIN_RES);
		bool onTerrain = true;
		for (uint32_t z = 0; z < TERRAIN_RES; z++)
		{
			for (uint32_t x = 0; x < TERRAIN_RES; x++)
			{
				Vector2f point = { -WORLD_HALF_TERRAIN_SIZE + x * WORLD_PART_SIZE, -WORLD_HALF_TERRAIN_SIZE + z * WORLD_PART_SIZE };
				Vector3f resultPoint;
				onTerrain = terrain->GetSurfacePoint(point.x, point.y, resultPoint);
				if (!onTerrain)
				{
					Vector3f last = allPoints.back();
					return false;
				}
				allPoints.push_back(resultPoint);
			}
		}
		return true;
	}

	bool TerrainTester::GetSurfacePoint_Test_BigAssertionTest(Terrain* terrain)
	{
		float step = WORLD_PART_SIZE - CORRECTIVE_VALUE;
		int32_t offset = 1024;
		int32_t border = int32_t(TERRAIN_RES + 1024);
		for (int32_t z = -offset; z < border; z++)
		{
			for (int32_t x = -offset; x < border; x++)
			{
				Vector3f resultPoint;
				bool result = terrain->GetSurfacePoint(x * step, z * step, resultPoint);
			}
		}
		return true;
	}

	void TerrainTester::PrintNotNullHeights(Terrain* terrain)
	{
		std::string output;
		output.reserve(TERRAIN_RES * TERRAIN_RES * 4);
		for (uint32_t i = 0; i < TERRAIN_RES; i++)
		{
			for (uint32_t j = 0; j < TERRAIN_RES; j++)
			{
				uint8_t h = terrain->m_heights[i * TERRAIN_RES + j];
				output += std::to_string(h) + " ";
			}
		}
		RN_APP_INFO("PrintNotNullHeights result: \n{0}\n", output);
	}

}