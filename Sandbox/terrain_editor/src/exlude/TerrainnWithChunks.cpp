
#include "Terrainn.h"
#include "Rainy/Render/Renderer.h"

namespace te {

	constexpr uint32_t MAX_CHUNKS_VAL = static_cast<uint32_t>(MAX_CHUNKS);
	
	constexpr uint32_t TERRAIN_RES = 8192 + 1;

	constexpr uint32_t PARTS_SECTION_SIZE = 32;

	constexpr uint32_t SECTION_CHUNK_SIZE = 8;
	constexpr uint32_t SECTION_TERRAIN_SIZE = MAX_CHUNKS_VAL * SECTION_CHUNK_SIZE;

	constexpr float WORLD_PART_SIZE = 1.f;
	constexpr float WORLD_SECTION_SIZE = (WORLD_PART_SIZE * (TERRAIN_RES - 1)) / SECTION_TERRAIN_SIZE;
	constexpr float WORLD_CHUNK_SIZE = SECTION_CHUNK_SIZE * WORLD_SECTION_SIZE;

	constexpr float WORLD_TERRAIN_SIZE = MAX_CHUNKS_VAL * WORLD_CHUNK_SIZE;
	constexpr float WORLD_HALF_TERRAIN_SIZE = WORLD_TERRAIN_SIZE / 2.f;

	constexpr uint32_t RAY_PARTS_COUNT = 1000;
	constexpr uint32_t RECURSION_COUNT = 100;

	constexpr float CORRECTIVE_VALUE = 0.001f;

	constexpr float TEX_CHUNK_SIZE = 1.f / static_cast<uint32_t>(MAX_CHUNKS);
	constexpr float TEX_SECTION_SIZE = TEX_CHUNK_SIZE / SECTION_CHUNK_SIZE;

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

	// *** TerrainChunk ***
	// public methods

	TerrainChunk::TerrainChunk(Vector2f posOffset, Vector2f texOffset)
	:	m_buffer(nullptr),
		m_sectionsCount(0),
		m_posOffset(posOffset),
		m_texOffset(texOffset)
	{
		m_sections.resize(SECTION_CHUNK_SIZE * SECTION_CHUNK_SIZE, nullptr);
	}

	TerrainChunk::~TerrainChunk()
	{
		delete m_buffer;
	}

	void TerrainChunk::Fill()
	{
		std::vector<TerrainSection> sectionsToDraw;
		sectionsToDraw.reserve(m_sections.capacity());
		for (float y = 0; y < float(SECTION_CHUNK_SIZE); y++)
		{
			for (float x = 0; x < float(SECTION_CHUNK_SIZE); x++)
			{
				TerrainSection* sectionPtr = new TerrainSection;
				sectionPtr->Position = Vector2f{ x, y } * WORLD_SECTION_SIZE + Vector2f{ m_posOffset };
				sectionPtr->TexturePosition = Vector2f{ x, y } * TEX_SECTION_SIZE + Vector2f{ m_texOffset };
				m_sections[uint32_t(y) * SECTION_CHUNK_SIZE + uint32_t(x)] = sectionPtr;
				sectionsToDraw.push_back(*sectionPtr);
			}
		}
		m_buffer = Rainy::VertexBuffer::Create(sizeof(TerrainSection) * sectionsToDraw.size(), sectionsToDraw.data());
		auto elemets = { Rainy::BufferElement(Rainy::FLOAT2, false, 1), Rainy::BufferElement(Rainy::FLOAT2, false, 1) };
		m_buffer->SetBufferLayout({ elemets,  sectionsToDraw.size() });
		m_sectionsCount = SECTION_CHUNK_SIZE * SECTION_CHUNK_SIZE;
	}

	TerrainSection* TerrainChunk::GetSection(Vector2f point)
	{
		if (!PointOnChunk(point)) return nullptr;

		point -= m_posOffset;

		Vector2i sectionCoord = ConvertToCoord(point, 0, WORLD_SECTION_SIZE);

		RN_ASSERT(sectionCoord.x >= 0, "sectionCoord.x < 0");
		RN_ASSERT(sectionCoord.y >= 0, "sectionCoord.y < 0");

		RN_ASSERT(sectionCoord.x < SECTION_CHUNK_SIZE, "sectionCoord.x > SECTION_CHUNK_SIZE");
		RN_ASSERT(sectionCoord.y < SECTION_CHUNK_SIZE, "sectionCoord.y > SECTION_CHUNK_SIZE");

		uint32_t sectionIndex = sectionCoord.y * SECTION_CHUNK_SIZE + sectionCoord.x;
		if (sectionIndex >= m_sections.size()) return nullptr;
		return m_sections[sectionIndex];
	}

	TerrainSection* TerrainChunk::GetSection(Vector2f point) const
	{
		if (!PointOnChunk(point)) return nullptr;

		point -= m_posOffset;

		Vector2i sectionCoord = ConvertToCoord(point, 0, WORLD_SECTION_SIZE);

		RN_ASSERT(sectionCoord.x >= 0, "sectionCoord.x < 0");
		RN_ASSERT(sectionCoord.y >= 0, "sectionCoord.y < 0");

		RN_ASSERT(sectionCoord.x < SECTION_CHUNK_SIZE, "sectionCoord.x > SECTION_CHUNK_SIZE");
		RN_ASSERT(sectionCoord.y < SECTION_CHUNK_SIZE, "sectionCoord.y > SECTION_CHUNK_SIZE");

		uint32_t sectionIndex = sectionCoord.y * SECTION_CHUNK_SIZE + sectionCoord.x;
		if (sectionIndex >= m_sections.size()) return nullptr;
		return m_sections[sectionIndex];
	}

	void TerrainChunk::StupidAddSection(Vector2f point)
	{
		if (GetSection(point) != nullptr) return;
		point -= m_posOffset;
		Vector2i sectionCoord = ConvertToCoord(point, 0, WORLD_SECTION_SIZE);
		Vector2f fsecCoord = { float(sectionCoord.x), float(sectionCoord.y) };
		TerrainSection* sectionPtr = new TerrainSection;
		sectionPtr->Position = fsecCoord * WORLD_SECTION_SIZE + Vector2f{ m_posOffset };
		sectionPtr->TexturePosition = fsecCoord * TEX_SECTION_SIZE + Vector2f{ m_texOffset };
		m_sections[sectionCoord.y * SECTION_CHUNK_SIZE + sectionCoord.x] = sectionPtr;
		m_sectionsCount++;

		RecreateBuffer();
	}

	void TerrainChunk::RemoveSection(Vector2f point)
	{
		if (GetSection(point) == nullptr) return;
		point -= m_posOffset;
		Vector2i sectionCoord = ConvertToCoord(point, 0, WORLD_SECTION_SIZE);
		uint32_t sectionIndex = sectionCoord.y * SECTION_CHUNK_SIZE + sectionCoord.x;
		delete m_sections[sectionIndex];
		m_sections[sectionIndex] = nullptr;
		m_sectionsCount--;

		RecreateBuffer();
	}

	// pivate methods

	bool TerrainChunk::PointOnChunk(Vector2f point) const
	{
		Vector2f topRightOffsets = { m_posOffset + Vector2f{ WORLD_CHUNK_SIZE } - point };
		Vector2f leftBottomOffsets = m_posOffset - point;
		if (leftBottomOffsets.x > 0 || topRightOffsets.x < 0 ||
			leftBottomOffsets.y > 0 || topRightOffsets.y < 0) return false;
		return true;
	}

	void TerrainChunk::RecreateBuffer()
	{
		delete m_buffer;
		std::vector<TerrainSection> sectionsToDraw;
		sectionsToDraw.reserve(m_sectionsCount);
		for (auto& sectionPtr : m_sections) { if (sectionPtr != nullptr) sectionsToDraw.push_back(*sectionPtr); }

		m_buffer = Rainy::VertexBuffer::Create(sizeof(TerrainSection) * sectionsToDraw.size(), sectionsToDraw.data());
		auto elemets = { Rainy::BufferElement(Rainy::FLOAT2, false, 1), Rainy::BufferElement(Rainy::FLOAT2, false, 1) };
		m_buffer->SetBufferLayout({ elemets,  sectionsToDraw.size() });
	}

	// *** Terrain ***
	// public methods

	Terrain::Terrain(TerrainChunks initSize)
	{
		m_partsInSection = PARTS_SECTION_SIZE;
		int32_t quadTerrainRes = TERRAIN_RES * TERRAIN_RES;
		m_heights.resize(quadTerrainRes, 0);
		m_normals.resize(quadTerrainRes, Rainy::Vector3<uint8_t>{ 0, 0, 255 });
		m_chunksMap.resize(MAX_CHUNKS_VAL * MAX_CHUNKS_VAL, nullptr);
		m_chunksToDraw.reserve(m_chunksMap.size());

		float u_initSize = float(static_cast<uint32_t>(initSize));
		
		float fhalfInitSize = u_initSize / 2;

		float posOffset = - WORLD_CHUNK_SIZE * fhalfInitSize;
		float texOffset = 0.5f - TEX_CHUNK_SIZE * fhalfInitSize;
		uint32_t chunkMapOffset = MAX_CHUNKS_VAL / 2 - fhalfInitSize;
		for (float y = 0; y < u_initSize; y++)
		{
			for (float x = 0; x < u_initSize; x++)
			{
				TerrainChunk* newChunkPtr = new TerrainChunk(
					Vector2f{ x, y } * WORLD_CHUNK_SIZE + Vector2f{ posOffset },
					Vector2f{ x, y } * TEX_CHUNK_SIZE + Vector2f{ texOffset }
				);
				newChunkPtr->Fill();
				uint32_t chunkIndex = (y + chunkMapOffset) * MAX_CHUNKS_VAL + x + chunkMapOffset;
				m_chunksMap[chunkIndex] = newChunkPtr;
				m_chunksToDraw.push_back(chunkIndex);
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
		
		m_indexBuffer = Rainy::IndexBuffer::Create(sizeof(uint32_t) * indices.size(), indices.size(), indices.data());
		m_vertexArray = Rainy::VertexArray::Create();
		
		m_vertexArray->SetIndexBuffer(m_indexBuffer);
		m_vertexArray->SetInstanceCount(SECTION_CHUNK_SIZE * SECTION_CHUNK_SIZE);
		
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
		for (auto chunkIndex : m_chunksToDraw)
		{
			auto chunkPtr = m_chunksMap[chunkIndex];
			if (chunkPtr == nullptr) continue;
			m_vertexArray->SetVertexBuffers({ m_vertexBuffer, chunkPtr->GetBuffer() });
			m_vertexArray->SetInstanceCount(chunkPtr->GetSectionsCount());

			m_shader->SetUniform("modelMatrix", GetModelMatrix());
			m_shader->SetUniform("maxTessLevel", float(m_partsInSection));
			m_shader->SetUniform("heightMap", int(m_heightMapTexture->GetTexUnit()));
			m_shader->SetUniform("normalMap", int(m_normalMapTexture->GetTexUnit()));
			m_shader->SetUniform("detailLevel", 25000.f);
			m_shader->SetUniform("heightFactor", 255.f);

			Rainy::DrawVertexArray(Rainy::RN_PATCHES, m_vertexArray);
		}
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

		TerrainChunk* chunkPtr = GetChunk(point);
		if (chunkPtr == nullptr)
		{
			Vector2f offsetPoint = { 
				point.x + WORLD_HALF_TERRAIN_SIZE, 
				point.y + WORLD_HALF_TERRAIN_SIZE 
			};
			Vector2i chunkCoord = ConvertToCoord(offsetPoint, 0, WORLD_CHUNK_SIZE);
			chunkPtr = new TerrainChunk(
				{ chunkCoord.x * WORLD_CHUNK_SIZE - WORLD_HALF_TERRAIN_SIZE, chunkCoord.y * WORLD_CHUNK_SIZE - WORLD_HALF_TERRAIN_SIZE },
				{ chunkCoord.x * TEX_CHUNK_SIZE, chunkCoord.y * TEX_CHUNK_SIZE }
			);
			uint32_t chunkIndex = chunkCoord.y * MAX_CHUNKS_VAL + chunkCoord.x;
			m_chunksMap[chunkIndex] = chunkPtr;
			m_chunksToDraw.push_back(chunkIndex);
		}
		chunkPtr->StupidAddSection(point);
	}

	void Terrain::RemoveSection(Vector2f point)
	{
		point.Mul(1.f / GetScale());
		TerrainChunk* chunkPtr = GetChunk(point);
		if (chunkPtr == nullptr) return;
		chunkPtr->RemoveSection(point);
		if (chunkPtr->GetSectionsCount() == 0)
		{
			Vector2f offsetPoint = {
				point.x + WORLD_HALF_TERRAIN_SIZE,
				point.y + WORLD_HALF_TERRAIN_SIZE
			};
			Vector2i chunkCoord = ConvertToCoord(offsetPoint, 0, WORLD_CHUNK_SIZE);
			uint32_t chunkIndex = chunkCoord.y * MAX_CHUNKS_VAL + chunkCoord.x;
			delete chunkPtr;
			m_chunksMap[chunkIndex] = nullptr;
		}
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
					terrainArea->Map.push_back(TerrainArea::INCORECT_INDEX);
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

		Vector2i firstIndex = area->FirstIndex;

		for (int32_t y = 0; y < area->Height; y++)
		{
			for (int32_t x = 0; x < area->Width; x++)
			{
				uint32_t indexFromMap = area->Map[y * area->Width + x];
				if (indexFromMap == TerrainArea::INCORECT_INDEX) continue;
				uint32_t terrIndex = (firstIndex.y + y) * TERRAIN_RES + firstIndex.x + x;
				RN_ASSERT(terrIndex <= (TERRAIN_RES * TERRAIN_RES), "ApplyChanges: terrIndex > (TERRAIN_RES * TERRAIN_RES)");
				m_heights[terrIndex] = area->Positions[indexFromMap].y;
				Vector3f n = area->Normals[indexFromMap] * 255.f;
				m_normals[terrIndex] = { uint8_t(n.x), uint8_t(n.y), uint8_t(n.z) };
			}
		}

	/*	m_heightMapTexture->TextureSubData(firstIndex.x, firstIndex.y,
			area->Width, area->Height, m_heights.data() + (firstIndex.y * TERRAIN_RES + firstIndex.x));
		m_normalMapTexture->TextureSubData(firstIndex.x, firstIndex.y,
			area->Width, area->Height, m_heights.data() + (firstIndex.y * TERRAIN_RES + firstIndex.x));*/
		m_heightMapTexture->TextureData(TERRAIN_RES, TERRAIN_RES, 1, Rainy::TextureDataType::UNSIGNED_BYTE, (void*)m_heights.data());
		m_normalMapTexture->TextureData(TERRAIN_RES, TERRAIN_RES, 3, Rainy::TextureDataType::UNSIGNED_BYTE, (void*)m_normals.data());
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

	TerrainChunk* Terrain::GetChunk(Vector2f point)
	{
		if (!PointOnTerrain(point.x, point.y)) return nullptr;

		point += { WORLD_HALF_TERRAIN_SIZE, WORLD_HALF_TERRAIN_SIZE };

		Vector2i chunkCoord = ConvertToCoord(point, 0, WORLD_CHUNK_SIZE);

		RN_ASSERT(chunkCoord.x >= 0, "chunkCoord.x < 0");
		RN_ASSERT(chunkCoord.y >= 0, "chunkCoord.y < 0");

		RN_ASSERT(chunkCoord.x < MAX_CHUNKS_VAL, "chunkCoord.x > HEIGHT_MAP_RES");
		RN_ASSERT(chunkCoord.y < MAX_CHUNKS_VAL, "chunkCoord.y > HEIGHT_MAP_RES");

		return m_chunksMap[chunkCoord.y * MAX_CHUNKS_VAL + chunkCoord.x];
	}

	TerrainChunk* Terrain::GetChunk(Vector2f point) const
	{
		if (!PointOnTerrain(point.x, point.y)) return nullptr;

		point += { WORLD_HALF_TERRAIN_SIZE, WORLD_HALF_TERRAIN_SIZE };

		Vector2i chunkCoord = ConvertToCoord(point, 0, WORLD_CHUNK_SIZE);

		RN_ASSERT(chunkCoord.x >= 0, "chunkCoord.x < 0");
		RN_ASSERT(chunkCoord.y >= 0, "chunkCoord.y < 0");

		RN_ASSERT(chunkCoord.x < MAX_CHUNKS_VAL, "chunkCoord.x > HEIGHT_MAP_RES");
		RN_ASSERT(chunkCoord.y < MAX_CHUNKS_VAL, "chunkCoord.y > HEIGHT_MAP_RES");

		return m_chunksMap[chunkCoord.y * MAX_CHUNKS_VAL + chunkCoord.x];
	}

	TerrainSection* Terrain::GetSection(Vector2f point)
	{
		TerrainChunk* chunk = GetChunk(point);
		if (chunk == nullptr) return nullptr;
		return chunk->GetSection(point);
	}

	TerrainSection* Terrain::GetSection(Vector2f point) const
	{
		TerrainChunk* chunk = GetChunk(point);
		if (chunk == nullptr) return nullptr;
		return chunk->GetSection(point);
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

	bool TerrainTester::GetSection_Test_AllOnTerrain(Terrain* terrain, TerrainChunks terrainInit)
	{
		uint32_t uHalfInitSize = static_cast<uint32_t>(terrainInit) / 2;
		float offset = uHalfInitSize * WORLD_SECTION_SIZE;
		Vector2f start = { -(uHalfInitSize) + WORLD_PART_SIZE, -(uHalfInitSize) + WORLD_PART_SIZE };
		Vector2f end = { uHalfInitSize - WORLD_PART_SIZE , uHalfInitSize - WORLD_PART_SIZE };
		bool result = true;
		for (float y = start.y; y < end.y; y += WORLD_PART_SIZE)
		{
			for (float x = start.x; x < end.x; x += WORLD_PART_SIZE)
			{
				if (terrain->GetSection({ x, y }) == nullptr) return false;
			}
		}
		return true;
	}

	bool TerrainTester::GetSection_Test_BigAssertionTest(Terrain* terrain, TerrainChunks terrainInit)
	{
		uint32_t uInitSize = static_cast<uint32_t>(terrainInit);
		float offset = uInitSize * WORLD_SECTION_SIZE;
		Vector2f start = { -offset, -offset };
		Vector2f end = { offset, offset };
		for (float y = start.y; y < end.y; y += WORLD_PART_SIZE)
		{
			for (float x = start.x; x < end.x; x += WORLD_PART_SIZE)
			{
				terrain->GetSection({ x, y });
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