#pragma once

#include <cstdint>
#include <vector>
#include "Rainy/Core/Math.h"
#include "Rainy/Render/Shader.h"
#include "Rainy/Render/Buffer.h"
#include "Rainy/Render/VertexArray.h"

using Rainy::Vector3f;
using Rainy::Vector2f;
using Rainy::Vector2i;
using Rainy::IndexBuffer;
using Rainy::VertexBuffer;
using Rainy::VertexArray;
using std::vector;

struct NewVertexLS
{
	Vector3f position;
	Vector3f normal;
	Vector2f tex_coord;
};

struct NewLandscapeSection
{
	bool selected = false;
	vector<uint32_t> uniq_indices;
	vector<uint32_t> indices;
	//uint32_t ibo;
	IndexBuffer* indexBuffer;
};

using NewVertexData = std::pair<NewVertexLS*, uint32_t>;

struct NewLandscapePiece
{
	uint32_t width{ 0 };
	uint32_t length{ 0 };

	vector<NewVertexData> vertices_data;
};

class NewLandscape
{
public:
	explicit NewLandscape(Vector3f position, uint16_t width, uint16_t lenght, 
		uint16_t section_size, float section_world_size);

	void Draw(Rainy::Shader* shader) const;

	bool RayIntersection(Vector3f& dest, Vector3f rayDirection,
		Vector3f rayStartPoint, float rayDistance) const;

	NewLandscapeSection* GetSection(Vector3f position) const;

	void AddSection(Vector3f position);

	NewLandscapePiece* GetTerrainArea(Vector3f const& center, float radius);

	void UpdateVertexBuffer(NewLandscapePiece * piece);

private:
	bool PointInXZRec(float x, float z) const;

	bool GetPointOnBorder(Vector3f lookAtVector, Vector3f rayStartPoint, Vector3f& dest) const;

	bool GetSurfacePoint(float x, float z, Vector3f& dest) const;

	bool PointBelow(Vector3f point) const;

	bool StartAboveEndBelow(Vector3f start, Vector3f end) const;

	Vector3f BinarySearch(Vector3f rayStartPoint, Vector3f ray,
		float start, float end, int recursion_count) const;

	void IncreaseMapSize();

	void ReloadVertexBuffer();

private:
	/*uint32_t vbo;
	uint32_t vao;*/
	VertexBuffer* vertexBuffer;
	VertexArray* vertexArray;

	vector<NewVertexLS> vertices;
	vector<NewLandscapeSection*> sections_map;
	vector<uint16_t> sections_to_draw;

	float vertex_step;
	// section size in world 1.f, 2.2f ...
	float section_world_size;
	// section size in segments (vertices = section_size + 1)
	uint16_t section_size;

	uint16_t map_width;
	uint16_t map_lenght;

	Vector3f m_position;
};