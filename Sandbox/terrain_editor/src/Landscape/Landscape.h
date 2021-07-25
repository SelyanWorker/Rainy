#pragma once

#include <vector>
#include <utility>

#include "Rainy/Core/Core.h"
#include "Rainy/Core/Entity.h"
#include "Rainy/Core/Math.h"
#include "Rainy/Render/Renderer.h"
#include "Rainy/Utils/Image.h"

#include "BrushAndTools/Brush.h"

using std::pair;
using std::vector;
using Rainy::Vector2i;
using Rainy::Vector2f;
using Rainy::Vector3f;

struct VertexLS;
struct Section;
class Landscape;

using VertexIndexType = uint32_t;
using SectionIndexType = uint16_t;
using VertexData = pair<VertexLS*, VertexIndexType>;
using SectionData = pair<Section*, SectionIndexType>;

struct VertexLS 
{
	Vector3f position;
	Vector3f normal;
	Vector2f tex_coord;
};

class Section 
{
public:
	vector<VertexIndexType> m_uniqIndices;
	Rainy::IndexBuffer* m_buffer;
};

struct LandscapePiece 
{
	uint32_t width{ 0 };
	uint32_t length{ 0 };

	vector<VertexData> vertices_data;
};

class Landscape : public Rainy::Entity
{
public:
	explicit Landscape(
		uint16_t width, uint16_t lenght, uint16_t sectionSize
	);

	//explicit Landscape(Rainy::Image * image, uint16_t width, uint16_t lenght, uint16_t section_size);

	void draw() const;

	bool rayIntersection(Vector3f & dest, Vector3f ray_direction, 
		Vector3f ray_start_point, float ray_distance) const;
		
	/*bool getSection(Vector2i position, SectionData & dest) override;

	bool getVertex(Vector2i position, VertexData & dest) override;*/
		 
	LandscapePiece* getTerrainArea(Vector3f const& center, float radius);

	void updateVBO(LandscapePiece * piece) const; // update VBO by LandscapePiece

	void toFlatten();

	void resize(uint32_t new_width, uint32_t new_height, float new_size_size);

	uint8_t* getRGBArray(uint16_t bpp) const;

private:
	bool pointInXZRec(float x, float z) const;

	bool getPointOnBorder(Vector3f lookAtVector, Vector3f rayStartPoint, Vector3f& dest) const;

	bool getSurfacePoint(float x, float z, Vector3f& dest) const;

	bool pointBelow(Vector3f point) const;

	bool startAboveEndBelow(Vector3f start, Vector3f end) const;

	Vector3f binarySearch(Vector3f ray_start_point, Vector3f ray,
		float start, float end, int recursion_count) const;

	void getMaxMinY(float& min_dest, float& max_dest) const;

private:
	vector<VertexLS> m_vertices;
	vector<Section*> m_sectionsMap;
	vector<uint32_t> m_sectionsToDraw;

	float m_sectionWorldSize;
	uint32_t m_sectionSize;

	uint32_t m_mapWidth;
	uint32_t m_mapLenght;
};


