#include "../include/terrain.h"
#include "core/time.h"
#include "render/renderer.h"

namespace te
{
    constexpr uint32_t MAX_INIT_SIZE_VAL = static_cast<uint32_t>(MAX_INIT_SIZE);

    constexpr uint32_t PARTS_SECTION_SIZE = 32;

    constexpr uint32_t TERRAIN_RES = MAX_INIT_SIZE_VAL * PARTS_SECTION_SIZE + 1;

    constexpr uint32_t SECTION_TERRAIN_SIZE = MAX_INIT_SIZE_VAL;

    constexpr float WORLD_PART_SIZE = 1.f;
    constexpr float WORLD_SECTION_SIZE = PARTS_SECTION_SIZE * WORLD_PART_SIZE;

    constexpr float WORLD_TERRAIN_SIZE = SECTION_TERRAIN_SIZE * WORLD_SECTION_SIZE;
    constexpr float WORLD_HALF_TERRAIN_SIZE = WORLD_TERRAIN_SIZE / 2.f;

    constexpr uint32_t RAY_PARTS_COUNT = 1000;
    constexpr uint32_t RECURSION_COUNT = 100;

    constexpr float CORRECTIVE_VALUE = 0.001f;

    constexpr float TEX_SECTION_SIZE = 1.f / SECTION_TERRAIN_SIZE;

    float HEIGHT_MOD = 1;

    Vector2i ToIntCoord(Vector2f floatCoord, Vector2f offset, float den)
    {
        return Vector2i{ int32_t((floatCoord.x + offset.x - CORRECTIVE_VALUE) / den),
                         int32_t((floatCoord.y + offset.y - CORRECTIVE_VALUE) / den) };
    }

    // *** Terrain ***
    // public methods

    Terrain::Terrain(TerrainInitSize columns,
                     TerrainInitSize rows,
                     Rainy::Image *heightMap,
                     Rainy::Image *normalsMap,
                     float heightMod)
    {
        Rainy::TimeStep time0 = Rainy::TimeStep::getTime();

        HEIGHT_MOD = heightMod;

        m_maxHeight = getPosition().y;
        m_minHeight = m_maxHeight;

        m_partsInSection = PARTS_SECTION_SIZE;
        int32_t quadTerrainRes = TERRAIN_RES * TERRAIN_RES;
        m_heights.resize(quadTerrainRes);
        m_normals.resize(quadTerrainRes, Vector3f{ 0, 0, 1.f });
        m_sectionsMap.resize(SECTION_TERRAIN_SIZE * SECTION_TERRAIN_SIZE);
        m_sectionsToDraw.reserve(m_sectionsMap.size());

        if (heightMap != nullptr && heightMap->GetChannels() == 1 &&
            heightMap->GetWidth() <= TERRAIN_RES && heightMap->GetWidth() <= TERRAIN_RES)
        {
            size_t halfTerrainRes = TERRAIN_RES / 2;
            size_t xOff = halfTerrainRes - heightMap->GetWidth() / 2;
            size_t yOff = halfTerrainRes - heightMap->GetHeight() / 2;
            const uint8_t *pixels = heightMap->GetData();
            for (size_t y = 0; y < heightMap->GetHeight(); y++)
            {
                for (size_t x = 0; x < heightMap->GetWidth(); x++)
                {
                    float height = float(pixels[y * heightMap->GetWidth() + x]) / 255.f;
                    m_heights[(y + yOff) * TERRAIN_RES + (x + xOff)] = height * heightMod;
                }
            }
        }

        float columnsCount = float(static_cast<uint32_t>(columns));
        float rowsCount = float(static_cast<uint32_t>(rows));
        float halfColumnsCount = columnsCount / 2;
        float halfRowsCount = rowsCount / 2;

        float posOffsetX = -WORLD_SECTION_SIZE * halfColumnsCount;
        float posOffsetY = -WORLD_SECTION_SIZE * halfRowsCount;

        float texOffsetU = 0.5f - TEX_SECTION_SIZE * halfColumnsCount;
        float texOffsetV = 0.5f - TEX_SECTION_SIZE * halfRowsCount;

        uint32_t sectionMapOffsetX = SECTION_TERRAIN_SIZE / 2 - halfColumnsCount;
        uint32_t sectionMapOffsetY = SECTION_TERRAIN_SIZE / 2 - halfRowsCount;
        for (float y = 0; y < rowsCount; y++)
        {
            for (float x = 0; x < columnsCount; x++)
            {
                TerrainSection *section = new TerrainSection();
                section->Position = Vector2f{ x * WORLD_SECTION_SIZE, y * WORLD_SECTION_SIZE } +
                                    Vector2f{ posOffsetX, posOffsetY };
                section->TexturePosition = Vector2f{ x * TEX_SECTION_SIZE, y * TEX_SECTION_SIZE } +
                                           Vector2f{ texOffsetU, texOffsetV };

                m_sectionsMap[(y + sectionMapOffsetY) * SECTION_TERRAIN_SIZE + x +
                              sectionMapOffsetX] = section;
                m_sectionsToDraw.push_back(*section);
            }
        }

        struct Vertex
        {
            Vector2f Position, TextureCoord;
        };
        std::array<Vertex, 4> vertices = {
            Vertex{ Vector2f{ 0.f, 0.f }, Vector2f{ 0.f, 0.f } },
            Vertex{ Vector2f{ 0.f, WORLD_SECTION_SIZE }, Vector2f{ 0.f, TEX_SECTION_SIZE } },
            Vertex{ Vector2f{ WORLD_SECTION_SIZE, WORLD_SECTION_SIZE },
                    Vector2f{ TEX_SECTION_SIZE, TEX_SECTION_SIZE } },
            Vertex{ Vector2f{ WORLD_SECTION_SIZE, 0.f }, Vector2f{ TEX_SECTION_SIZE, 0.f } }
        };

        std::array<uint32_t, 4> indices = { 0, 1, 2, 3 };
        // std::array<uint32_t, 4> indices = { 0, 3, 2, 1 };

        m_vertexBuffer =
            Rainy::VertexBuffer::Create(sizeof(Vertex) * vertices.size(), vertices.data());
        auto elements_0 = { Rainy::BufferElement(Rainy::FLOAT2, false),
                            Rainy::BufferElement(Rainy::FLOAT2, false) };
        m_vertexBuffer->SetBufferLayout({ elements_0, vertices.size() });

        m_sectionBuffer =
            Rainy::VertexBuffer::Create(sizeof(TerrainSection) * m_sectionsMap.size(), nullptr);
        m_sectionBuffer->SubData(0,
                                 sizeof(TerrainSection) * m_sectionsToDraw.size(),
                                 m_sectionsToDraw.data());
        auto elements_1 = { Rainy::BufferElement(Rainy::FLOAT2, false, 1),
                            Rainy::BufferElement(Rainy::FLOAT2, false, 1) };
        m_sectionBuffer->SetBufferLayout({ elements_1, m_sectionsToDraw.size() });

        m_indexBuffer = Rainy::IndexBuffer::Create(sizeof(uint32_t) * indices.size(),
                                                   indices.size(),
                                                   indices.data());

        m_vertexArray = Rainy::VertexArray::Create();
        m_vertexArray->SetIndexBuffer(m_indexBuffer);
        m_vertexArray->SetVertexBuffers({ m_vertexBuffer, m_sectionBuffer });
        m_vertexArray->SetInstanceCount(m_sectionsToDraw.size());

        Rainy::Image *image = Rainy::Image::Create("res/textures/hm.jpg");
        m_heightMapTexture = Texture2D::Create();
        // m_heightMapTexture->TextureData(image);
        m_heightMapTexture->TextureData(TERRAIN_RES,
                                        TERRAIN_RES,
                                        1,
                                        Rainy::TextureDataType::FLOAT,
                                        (void *)m_heights.data());
        m_heightMapTexture->SetTexUnit(0);
        delete image;

        image = Rainy::Image::Create("res/textures/nm.jpg");
        m_normalMapTexture = Texture2D::Create();
        // m_normalMapTexture->TextureData(image);
        m_normalMapTexture->TextureData(TERRAIN_RES,
                                        TERRAIN_RES,
                                        3,
                                        Rainy::TextureDataType::FLOAT,
                                        (void *)m_normals.data());
        m_normalMapTexture->SetTexUnit(m_heightMapTexture->GetTexUnit() + 1);
        delete image;

        RN_APP_INFO("Terrain generation by {0} s.",
                    Rainy::TimeStep::getTime().getSeconds() - time0.getSeconds());
    }

    Terrain::~Terrain()
    {
        delete m_vertexArray;
        delete m_indexBuffer;
        delete m_vertexBuffer;
    }

    void Terrain::Draw(Rainy::Shader *terrainShader)
    {
        m_heightMapTexture->Bind();
        m_normalMapTexture->Bind();
        terrainShader->Bind();

        terrainShader->SetUniform("modelMatrix", getModelMatrix());
        terrainShader->SetUniform("maxTessLevel", float(m_partsInSection));
        terrainShader->SetUniform("heightMap", int(m_heightMapTexture->GetTexUnit()));
        terrainShader->SetUniform("normalMap", int(m_normalMapTexture->GetTexUnit()));
        terrainShader->SetUniform("detailLevel", 25000.f);
        terrainShader->SetUniform("heightFactor", 1.f);

        Rainy::DrawVertexArray(Rainy::RN_PATCHES, m_vertexArray);

        terrainShader->UnBind();
        m_normalMapTexture->UnBind();
        m_heightMapTexture->UnBind();
    }

    bool Terrain::RayIntersection(Vector3f &dest,
                                  Vector3f rayDirection,
                                  Vector3f rayStartPoint,
                                  float rayDistance) const
    {
        float scale = GetScale();
        rayStartPoint.mul(1.f / scale);

        Vector3f startPoint;
        GetPointOnBorder(rayDirection, rayStartPoint, startPoint);
        Vector3f endPoint;
        GetPointOnBorder(rayDirection * (-1.f),
                         rayStartPoint + (rayDirection * rayDistance),
                         endPoint);

        float partLength = (endPoint - startPoint).length() / RAY_PARTS_COUNT;

        if (StartAboveEndBelow(startPoint, endPoint))
        {
            for (uint32_t i = 0; i < RAY_PARTS_COUNT; ++i)
            {
                Vector3f startRayPartPoint = startPoint + rayDirection * (partLength * i);
                Vector3f endRayPartPoint = startPoint + rayDirection * (partLength * (i + 1));

                if (StartAboveEndBelow(startRayPartPoint, endRayPartPoint))
                {
                    dest = BinarySearch(startRayPartPoint,
                                        rayDirection,
                                        0,
                                        partLength,
                                        RECURSION_COUNT);
                    bool hasSection = GetSection({ dest.x, dest.z }) != nullptr;
                    // dest.mul(scale);
                    return hasSection;
                }
            }
        }

        return false;
    }

    void Terrain::AddSection(Vector2f point)
    {
        point.Mul(1.f / GetScale());
        if (GetSection(point) != nullptr)
            return;

        std::array<Vector2f, 4> adjacentPoints = {
            Vector2f{ point.x, point.y - WORLD_SECTION_SIZE },
            Vector2f{ point.x, point.y + WORLD_SECTION_SIZE },
            Vector2f{ point.x - WORLD_SECTION_SIZE, point.y },
            Vector2f{ point.x + WORLD_SECTION_SIZE, point.y }
        };

        bool hasBottom = GetSection(adjacentPoints[0]) == nullptr;
        bool hasTop = GetSection(adjacentPoints[1]) == nullptr;
        bool hasLeft = GetSection(adjacentPoints[2]) == nullptr;
        bool hasRight = GetSection(adjacentPoints[3]) == nullptr;

        if (GetSection(adjacentPoints[0]) == nullptr &&
            GetSection(adjacentPoints[1]) == nullptr &&
            GetSection(adjacentPoints[2]) == nullptr && GetSection(adjacentPoints[3]) == nullptr)
            return;

        uint32_t prevToDrawCount = m_sectionsToDraw.size();

        Vector2i newSectionCoord =
            ToIntCoord(point, Vector2f{ WORLD_HALF_TERRAIN_SIZE }, WORLD_SECTION_SIZE);
        TerrainSection *newSection = new TerrainSection();
        newSection->Position = { newSectionCoord.x * WORLD_SECTION_SIZE - WORLD_HALF_TERRAIN_SIZE,
                                 newSectionCoord.y * WORLD_SECTION_SIZE - WORLD_HALF_TERRAIN_SIZE };
        newSection->TexturePosition = { newSectionCoord.x * TEX_SECTION_SIZE,
                                        newSectionCoord.y * TEX_SECTION_SIZE };

        m_sectionsMap[newSectionCoord.y * SECTION_TERRAIN_SIZE + newSectionCoord.x] = newSection;
        m_sectionsToDraw.push_back(*newSection);

        m_sectionBuffer->SubData(sizeof(TerrainSection) * prevToDrawCount,
                                 sizeof(TerrainSection),
                                 newSection);

        m_vertexArray->SetInstanceCount(m_sectionsToDraw.size());
    }

    void Terrain::RemoveSection(Vector2f point)
    {
        point.Mul(1.f / GetScale());
        TerrainSection *section = GetSection(point);
        if (section == nullptr)
            return;
        delete section;
        Vector2i sectionCoord =
            ToIntCoord(point, Vector2f{ WORLD_HALF_TERRAIN_SIZE }, WORLD_SECTION_SIZE);
        m_sectionsMap[sectionCoord.y * SECTION_TERRAIN_SIZE + sectionCoord.x] = nullptr;

        m_sectionsToDraw.clear();
        for (auto sectionPtr : m_sectionsMap)
        {
            if (sectionPtr != nullptr)
                m_sectionsToDraw.push_back(*sectionPtr);
        }

        m_sectionBuffer->Reallocate(sizeof(TerrainSection) * m_sectionsMap.size(), nullptr);
        m_sectionBuffer->SubData(0,
                                 sizeof(TerrainSection) * m_sectionsToDraw.size(),
                                 m_sectionsToDraw.data());
        m_vertexArray->SetInstanceCount(m_sectionsToDraw.size());
    }

    TerrainArea *Terrain::GetArea(Vector2f point, float radius)
    {
        Rainy::AABB2Df recArea = { { point.x - radius + WORLD_HALF_TERRAIN_SIZE,
                                     point.y - radius + WORLD_HALF_TERRAIN_SIZE },
                                   { point.x + radius + WORLD_HALF_TERRAIN_SIZE,
                                     point.y + radius + WORLD_HALF_TERRAIN_SIZE } };
        if (recArea.Min.x < 0)
            recArea.Min.x = 0;
        if (recArea.Max.x > WORLD_TERRAIN_SIZE)
            recArea.Max.x = WORLD_TERRAIN_SIZE;
        if (recArea.Min.y < 0)
            recArea.Min.y = 0;
        if (recArea.Max.y > WORLD_TERRAIN_SIZE)
            recArea.Max.y = WORLD_TERRAIN_SIZE;

        Vector2i firstCoord = { int(recArea.Min.x / WORLD_PART_SIZE),
                                int(recArea.Min.y / WORLD_PART_SIZE) };
        Vector2i lastCoord = { int(recArea.Max.x / WORLD_PART_SIZE),
                               int(recArea.Max.y / WORLD_PART_SIZE) };

        TerrainArea *terrainArea = new TerrainArea({ firstCoord.x, firstCoord.y });
        terrainArea->Width = lastCoord.x - firstCoord.x + 1;
        terrainArea->Height = lastCoord.y - firstCoord.y + 1;
        terrainArea->Positions.reserve(terrainArea->Height * terrainArea->Width);
        terrainArea->Normals.reserve(terrainArea->Height * terrainArea->Width);

        for (int32_t y = firstCoord.y; y <= lastCoord.y; y++)
        {
            for (int32_t x = firstCoord.x; x <= lastCoord.x; x++)
            {
                Vector2f currPoint2d = { x * WORLD_PART_SIZE - WORLD_HALF_TERRAIN_SIZE,
                                         y * WORLD_PART_SIZE - WORLD_HALF_TERRAIN_SIZE };
                TerrainSection *section = GetSection(currPoint2d);
                if (section == nullptr)
                {
                    terrainArea->Positions.push_back({});
                    terrainArea->Normals.push_back({});
                    terrainArea->Map.push_back(TerrainArea::INCORRECT_INDEX);
                }
                else
                {
                    Vector3f position = { currPoint2d.x,
                                          m_heights[y * TERRAIN_RES + x],
                                          currPoint2d.y };
                    auto &n = m_normals[y * TERRAIN_RES + x];
                    Vector3f normal = Vector3f{ float(n.x), float(n.y), float(n.z) } / 255.f;
                    terrainArea->Positions.push_back(position);
                    terrainArea->Normals.push_back(normal);
                    terrainArea->Map.push_back(terrainArea->Positions.size() - 1);
                }
            }
        }

        RN_ASSERT((terrainArea->FirstIndex.x + terrainArea->Width - 1) <= TERRAIN_RES,
                  "GetArea: (terrainArea->FirstIndex.x + terrainArea->Width - 1) > TERRAIN_RES");
        RN_ASSERT((terrainArea->FirstIndex.y + terrainArea->Height - 1) <= TERRAIN_RES,
                  "GetArea: (terrainArea->FirstIndex.y + terrainArea->Width - 1) > TERRAIN_RES");

        return terrainArea;
    }

    void Terrain::ApplyArea(TerrainArea *area)
    {
        RN_ASSERT(area != nullptr, "ApplyChanges: area == nullptr");

        Rainy::TimeStep time0 = Rainy::TimeStep::getTime();
        Vector2i firstIndex = area->FirstIndex;
        for (int32_t y = 0; y < area->Height; y++)
        {
            for (int32_t x = 0; x < area->Width; x++)
            {
                uint32_t indexFromMap = area->Map[y * area->Width + x];
                if (indexFromMap == TerrainArea::INCORRECT_INDEX)
                    continue;
                uint32_t terrIndex = (firstIndex.y + y) * TERRAIN_RES + firstIndex.x + x;

                RN_ASSERT(terrIndex <= (TERRAIN_RES * TERRAIN_RES),
                          "ApplyChanges: terrIndex > (TERRAIN_RES * TERRAIN_RES)");

                m_heights[terrIndex] = area->Positions[indexFromMap].y;
                if (m_heights[terrIndex] > m_maxHeight)
                    m_maxHeight = m_heights[terrIndex];
                else if (m_heights[terrIndex] < m_minHeight)
                    m_minHeight = m_heights[terrIndex];

                Vector3f n = area->Normals[indexFromMap];
                m_normals[terrIndex] = n;
            }
        }

        for (uint32_t y = firstIndex.y; y <= (firstIndex.y + area->Height); y++)
        {
            m_heightMapTexture->TextureSubData(firstIndex.x,
                                               y,
                                               area->Width,
                                               1,
                                               m_heights.data() + (y * TERRAIN_RES + firstIndex.x));
            m_normalMapTexture->TextureSubData(firstIndex.x,
                                               y,
                                               area->Width,
                                               1,
                                               m_normals.data() + (y * TERRAIN_RES + firstIndex.x));
        }
    }

    void Terrain::ApplyAreaParts(TerrainArea *area, std::vector<Rainy::AABB2Di> parts)
    {
        Vector2i firstIndex = area->FirstIndex;
        for (Rainy::AABB2Di part : parts)
        {
            for (int32_t y = part.Min.y; y < part.Max.y; y++)
            {
                for (int32_t x = part.Min.x; x < part.Max.x; x++)
                {
                    uint32_t indexFromMap = area->Map[y * area->Width + x];

                    if (indexFromMap == TerrainArea::INCORRECT_INDEX)
                        continue;

                    uint32_t terrIndex = (firstIndex.y + y) * TERRAIN_RES + firstIndex.x + x;
                    RN_ASSERT(terrIndex <= (TERRAIN_RES * TERRAIN_RES),
                              "ApplyChanges: terrIndex > (TERRAIN_RES * TERRAIN_RES)");

                    m_heights[terrIndex] = area->Positions[indexFromMap].y;
                    if (m_heights[terrIndex] > m_maxHeight)
                        m_maxHeight = m_heights[terrIndex];
                    else if (m_heights[terrIndex] < m_minHeight)
                        m_minHeight = m_heights[terrIndex];

                    m_normals[terrIndex] = area->Normals[indexFromMap];
                }
            }
        }

        for (uint32_t y = firstIndex.y; y <= (firstIndex.y + area->Height); y++)
        {
            m_heightMapTexture->TextureSubData(firstIndex.x,
                                               y,
                                               area->Width,
                                               1,
                                               m_heights.data() + (y * TERRAIN_RES + firstIndex.x));
            m_normalMapTexture->TextureSubData(firstIndex.x,
                                               y,
                                               area->Width,
                                               1,
                                               m_normals.data() + (y * TERRAIN_RES + firstIndex.x));
        }
    }

    Rainy::Image *Terrain::GetHeightMapImage() const
    {
        Rainy::AABB2Di mapBorders = GetRealMapBorders();
        mapBorders.Min.Mul(PARTS_SECTION_SIZE);
        mapBorders.Max.Mul(PARTS_SECTION_SIZE);

        mapBorders.Max += Vector2i{ 1 };

        std::vector<uint8_t> bytes;
        auto maxSubMin = (mapBorders.Max - mapBorders.Min);
        bytes.resize(maxSubMin.x * maxSubMin.y);

        float heightDiff = std::fabs(m_maxHeight - m_minHeight);
        if (heightDiff == 0)
            heightDiff = 1;
        if (heightDiff < HEIGHT_MOD)
            heightDiff = HEIGHT_MOD;
        for (int32_t y = mapBorders.Min.y; y < mapBorders.Max.y; y++)
        {
            for (int32_t x = mapBorders.Min.x; x < mapBorders.Max.x; x++)
            {
                float h = m_heights[y * TERRAIN_RES + x] / heightDiff * 255;
                bytes[(y - mapBorders.Min.y) * maxSubMin.x + (x - mapBorders.Min.x)] = uint8_t(h);
            }
        }
        return Rainy::Image::Create(bytes.data(), 1, maxSubMin.x, maxSubMin.y);
    }

    Rainy::Image *Terrain::GetNormalsMapImage() const
    {
        Rainy::AABB2Di mapBorders = GetRealMapBorders();
        mapBorders.Min.Mul(PARTS_SECTION_SIZE);
        mapBorders.Max.Mul(PARTS_SECTION_SIZE);

        mapBorders.Max += Vector2i{ 1 };

        std::vector<uint8_t> bytes;
        auto maxSubMin = (mapBorders.Max - mapBorders.Min);
        bytes.resize(maxSubMin.x * maxSubMin.y * 3);

        for (int32_t y = mapBorders.Min.y; y < mapBorders.Max.y; y++)
        {
            for (int32_t x = mapBorders.Min.x; x < mapBorders.Max.x; x++)
            {
                auto GetHeight = [this](int32_t x, int32_t y, int32_t xOff, int32_t yOff)
                {
                    int32_t newX = std::clamp<int32_t>(x + xOff, 0, TERRAIN_RES - 1);
                    int32_t newY = std::clamp<int32_t>(y + yOff, 0, TERRAIN_RES - 1);
                    return m_heights[newY * TERRAIN_RES + newX];
                };

                float h11 = GetHeight(x, y, 0, 0);
                float h01 = GetHeight(x, y, -1, 0);
                float h21 = GetHeight(x, y, 1, 0);
                float h10 = GetHeight(x, y, 0, -1);
                float h12 = GetHeight(x, y, 0, 1);

                Vector3f va{ 2.0, 0, h21 - h01 };
                va.normalize();

                Vector3f vb{ 0.0, 2, h12 - h10 };
                vb.normalize();

                Vector3f normal = va.cross(vb);

                size_t i = ((y - mapBorders.Min.y) * maxSubMin.x + (x - mapBorders.Min.x)) * 3;

                normal.y *= -1.f;

                // [-1, 1] to [0, 1]
                normal.mul(0.5f);
                normal.add({ 0.5f });

                bytes[i + 0] = uint8_t(normal.x * 255.f);
                bytes[i + 1] = uint8_t(normal.y * 255.f);
                bytes[i + 2] = uint8_t(normal.z * 255.f);

                /*bytes.push_back(uint8_t(normal.x * 255.f));
                bytes.push_back(uint8_t(normal.z * 255.f));
                bytes.push_back(uint8_t(normal.x * 255.f));*/

                /*
                const vec2 size = vec2(2.0,0.0);
                const ivec3 off = ivec3(-1,0,1);

                float s11 = newPosition.y;
                float s01 = textureOffset(heightMap, newTexCoord, off.xy).r * heightFactor;
                float s21 = textureOffset(heightMap, newTexCoord, off.zy).r * heightFactor;
                float s10 = textureOffset(heightMap, newTexCoord, off.yx).r * heightFactor;
                float s12 = textureOffset(heightMap, newTexCoord, off.yz).r * heightFactor;

                vec3 va = normalize(vec3(size.xy, s21 - s01));
                vec3 vb = normalize(vec3(size.yx, s12 - s10));

                output.normal0 = cross(va, vb).xzy;*/
            }
        }
        return Rainy::Image::Create(bytes.data(), 3, maxSubMin.x, maxSubMin.y);
    }

    void Terrain::Flat()
    {
        float height = getPosition().y;
        for (auto &h : m_heights)
            h = height;
        m_heightMapTexture->TextureSubData(0, 0, TERRAIN_RES, TERRAIN_RES, m_heights.data());
    }

    // private methods

    bool Terrain::PointOnTerrain(float x, float z) const
    {
        Vector3f position = getPosition();
        float topRightXOffset = position.x + (WORLD_HALF_TERRAIN_SIZE)-x;
        float topRightZOffset = position.z + (WORLD_HALF_TERRAIN_SIZE)-z;

        float leftBottomXOffset = position.x - (WORLD_HALF_TERRAIN_SIZE)-x;
        float leftBottomZOffset = position.z - (WORLD_HALF_TERRAIN_SIZE)-z;

        if (leftBottomXOffset > 0 || topRightXOffset < 0 || leftBottomZOffset > 0 ||
            topRightZOffset < 0)
            return false;

        return true;
    }

    bool Terrain::GetPointOnBorder(Vector3f dir, Vector3f start, Vector3f &dest) const
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
        if (interRayAndSurface({ 0, 0, -WORLD_HALF_TERRAIN_SIZE },
                               { 0, 0, -1 },
                               start,
                               dir,
                               intersectionPoint,
                               rayLength))
        {
            if (-(WORLD_HALF_TERRAIN_SIZE + intersectionPoint.z) <= CORRECTIVE_VALUE)
                intersectionPoint.z = -WORLD_HALF_TERRAIN_SIZE;
            pointProcess();
        }

        if (interRayAndSurface({ -WORLD_HALF_TERRAIN_SIZE, 0, 0 },
                               { -1, 0, 0 },
                               start,
                               dir,
                               intersectionPoint,
                               rayLength))
        {
            if (-(WORLD_HALF_TERRAIN_SIZE + intersectionPoint.x) <= CORRECTIVE_VALUE)
                intersectionPoint.x = -WORLD_HALF_TERRAIN_SIZE;
            pointProcess();
        }

        // right top
        if (interRayAndSurface({ 0, 0, WORLD_HALF_TERRAIN_SIZE },
                               { 0, 0, 1 },
                               start,
                               dir,
                               intersectionPoint,
                               rayLength))
        {
            if ((intersectionPoint.z - WORLD_HALF_TERRAIN_SIZE) <= CORRECTIVE_VALUE)
                intersectionPoint.z = WORLD_HALF_TERRAIN_SIZE;
            pointProcess();
        }

        if (interRayAndSurface({ WORLD_HALF_TERRAIN_SIZE, 0, 0 },
                               { 1, 0, 0 },
                               start,
                               dir,
                               intersectionPoint,
                               rayLength))
        {
            if ((intersectionPoint.x - WORLD_HALF_TERRAIN_SIZE) <= CORRECTIVE_VALUE)
                intersectionPoint.x = WORLD_HALF_TERRAIN_SIZE;
            pointProcess();
        }

        if (minRayLength < 0)
            return false;

        return true;
    }

    bool Terrain::GetSurfacePoint(float x, float z, Vector3f &dest) const
    {
        if (!PointOnTerrain(x, z))
            return false;

        Vector2f pointWithIndent = { x + WORLD_HALF_TERRAIN_SIZE, z + WORLD_HALF_TERRAIN_SIZE };

        Vector2i heightCoord = ToIntCoord(pointWithIndent, { 0 }, WORLD_PART_SIZE);

        RN_ASSERT(heightCoord.x >= 0, "vertexXPos < 0");
        RN_ASSERT(heightCoord.y >= 0, "vertexZPos < 0");

        RN_ASSERT(heightCoord.x < (TERRAIN_RES - 1), "vertexXPosI > HEIGHT_MAP_RES");
        RN_ASSERT(heightCoord.y < (TERRAIN_RES - 1), "vertexZPosI > HEIGHT_MAP_RES");

        Vector3f v0 = { 0, m_heights[heightCoord.y * TERRAIN_RES + heightCoord.x], 0 };
        Vector3f v1 = { 0,
                        m_heights[(heightCoord.y + 1) * TERRAIN_RES + heightCoord.x],
                        WORLD_PART_SIZE };
        Vector3f v2 = { WORLD_PART_SIZE,
                        m_heights[heightCoord.y * TERRAIN_RES + (heightCoord.x + 1)],
                        0 };

        float vertexXPosFloat = float(heightCoord.x) * WORLD_PART_SIZE;
        float vertexZPosFloat = float(heightCoord.y) * WORLD_PART_SIZE;

        Vector2f localPosition = pointWithIndent - Vector2f{ vertexXPosFloat, vertexZPosFloat };

        if (localPosition.x > (WORLD_PART_SIZE - (localPosition.y)))
            v0 = { WORLD_PART_SIZE,
                   m_heights[(heightCoord.y + 1) * TERRAIN_RES + (heightCoord.x + 1)],
                   WORLD_PART_SIZE };

        Vector3f weights = barycentricCoord(localPosition, v0, v1, v2);

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

    Vector3f Terrain::BinarySearch(Vector3f rayStartPoint,
                                   Vector3f rayDirection,
                                   float start,
                                   float end,
                                   int recursionCount) const
    {
        /*RN_CORE_TRACE("RecCount: {0}", recursion_count);*/
        float half = start + (end - start) / 2;

        Vector3f rayHalfLengthPoint = rayStartPoint + rayDirection * half;

        if (recursionCount <= 0)
        {
            Vector3f surfacePoint;
            GetSurfacePoint(rayHalfLengthPoint.x, rayHalfLengthPoint.z, surfacePoint);
            return surfacePoint;
        }

        if (StartAboveEndBelow(rayStartPoint + rayDirection * start, rayHalfLengthPoint))
            return BinarySearch(rayStartPoint, rayDirection, start, half, recursionCount - 1);
        else
            return BinarySearch(rayStartPoint, rayDirection, half, end, recursionCount - 1);
    }

    TerrainSection const *Terrain::GetSection(Vector2f point) const
    {
        if (!PointOnTerrain(point.x, point.y))
            return nullptr;

        Vector2i sectionCoord =
            ToIntCoord(point, Vector2f{ WORLD_HALF_TERRAIN_SIZE }, WORLD_SECTION_SIZE);

        RN_ASSERT(sectionCoord.x < SECTION_TERRAIN_SIZE, "sectionCoord.x >= SECTION_TERRAIN_SIZE");
        RN_ASSERT(sectionCoord.y < SECTION_TERRAIN_SIZE, "sectionCoord.y >= SECTION_TERRAIN_SIZE");

        return m_sectionsMap[sectionCoord.y * SECTION_TERRAIN_SIZE + sectionCoord.x];
    }

    TerrainSection *Terrain::GetSection(Vector2f point)
    {
        return const_cast<TerrainSection *>(const_cast<const Terrain *>(this)->GetSection(point));
    }

    Rainy::AABB2Di Terrain::GetRealMapBorders() const
    {
        Rainy::AABB2Di borders{ { int32_t(SECTION_TERRAIN_SIZE) - 1,
                                  int32_t(SECTION_TERRAIN_SIZE) - 1 },
                                { 0, 0 } };
        for (size_t y = 0; y < SECTION_TERRAIN_SIZE; y++)
        {
            for (size_t x = 0; x < SECTION_TERRAIN_SIZE; x++)
            {
                if (m_sectionsMap[y * SECTION_TERRAIN_SIZE + x] == nullptr)
                    continue;

                if (borders.Min.x > x)
                    borders.Min.x = x;
                else if (borders.Max.x < x)
                    borders.Max.x = x;

                if (borders.Min.y > y)
                    borders.Min.y = y;
                else if (borders.Max.y < y)
                    borders.Max.y = y;
            }
        }
        borders.Max.add({ 1, 1 });
        return borders;
    }

    bool TerrainTester::PointInMap_Test(Terrain *terrain) { return true; }

    bool TerrainTester::GetSurfacePoint_Test_AllPointsOnTerrain(Terrain *terrain)
    {
        std::vector<Vector3f> allPoints;
        allPoints.reserve(TERRAIN_RES * TERRAIN_RES);
        bool onTerrain = true;
        for (uint32_t z = 0; z < TERRAIN_RES; z++)
        {
            for (uint32_t x = 0; x < TERRAIN_RES; x++)
            {
                Vector2f point = { -WORLD_HALF_TERRAIN_SIZE + x * WORLD_PART_SIZE,
                                   -WORLD_HALF_TERRAIN_SIZE + z * WORLD_PART_SIZE };
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

    bool TerrainTester::GetSurfacePoint_Test_BigAssertionTest(Terrain *terrain)
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

    void TerrainTester::PrintNotNullHeights(Terrain *terrain)
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
