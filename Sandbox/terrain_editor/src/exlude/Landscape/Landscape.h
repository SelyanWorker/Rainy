#pragma once

#include <vector>
#include <utility>

#include "Rainy/Core/Core.h"
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
class ITerrain;
struct LandscapePiece;
struct TerrainSection;
class Landscape;

using VertexIndexType = uint32_t;
using SectionIndexType = uint16_t;
using VertexData = pair<VertexLS*, VertexIndexType>;
using SectionData = pair<TerrainSection*, SectionIndexType>;

struct VertexLS 
{
	Vector3f position;
	Vector3f normal;
	Vector2f tex_coord;
};

struct TerrainSection 
{
	vector<VertexIndexType> uniq_indices;
	vector<VertexIndexType> indices;
	uint32_t ibo;
};

struct LandscapePiece 
{
	uint32_t width{ 0 };
	uint32_t length{ 0 };

	vector<VertexData> vertices_data;
};

class Landscape 
{
public:
	explicit Landscape(
		Vector3f position, uint16_t width, uint16_t lenght, uint16_t section_size, float section_world_size
	);

	explicit Landscape(Rainy::Image * image, uint16_t width, uint16_t lenght, uint16_t section_size);

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
	uint32_t vbo{ 0 };
	uint32_t vao{ 0 };

	vector<VertexLS> vertices;
	vector<TerrainSection*> sections_map;
	vector<uint16_t> sections_to_draw;

	float vertex_step;
	// section size in world 1.f, 2.2f ...
	float section_world_size;
	// section size in segments (vertices = section_size + 1)
	uint16_t section_size;

	uint16_t map_width;
	uint16_t map_lenght;

	Vector3f position{ 0 };
};


