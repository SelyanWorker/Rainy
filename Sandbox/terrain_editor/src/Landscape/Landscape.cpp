#include <algorithm>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>

#include "Rainy/Core/Core.h"
#include "Rainy/Render/Renderer.h"
#include "Landscape.h"
#include "Config.h"

constexpr uint32_t SECTION_RESTART_INDEX = 0u - 1;
constexpr uint8_t RAY_PARTS_COUNT = 1000;
constexpr float SECTION_PART_SIZE = 1.f;

Landscape::Landscape(uint16_t width, uint16_t lenght, uint16_t sectionSize)
:	m_sectionSize(sectionSize), 
	m_sectionWorldSize(m_sectionWorldSize)
{
	const uint32_t verticesCount = m_sectionSize + 1;
	const uint32_t vertices_rows = lenght * m_sectionSize;
	const uint32_t vertices_columns = width * m_sectionSize;

	vertices.reserve(vertices_rows * vertices_columns);

	Vector3f terrain_offset = 
		position - Vector3f{ width * section_world_size / 2, 0,  lenght * section_world_size / 2 };

	for (uint32_t i = 0; i < vertices_rows; ++i) {
		for (uint32_t j = 0; j < vertices_columns; ++j) {
			vertices.push_back({
				Vector3f{ j * vertex_step, 0, i * vertex_step } + terrain_offset,
				{ 0, 1, 0 },
				{ 0, 0 }
			});
		}
	}

	map_width = width * 2;
	map_lenght = lenght * 2;

	sections_map.resize(map_lenght * map_width, nullptr);
	sections_to_draw.reserve(lenght * width);

	Vector2i map_offset = { width / 2, lenght / 2 };

	for (uint16_t i = 0; i < lenght; ++i) {
		for (uint16_t j = 0; j < width; ++j) {
			Section * section = new Section();
			uint16_t map_index = (map_offset.y + i) * map_width + map_offset.x + j;
			sections_map[map_index] = section;
			sections_to_draw.push_back(map_index);

			for (uint16_t ii = 0; ii <= section_size; ++ii) 
			{
				for (uint16_t jj = 0; jj <= section_size; ++jj) 
				{
					section->uniq_indices.push_back(
						(i * section_size + ii) * vertices_columns +
						(j * section_size + jj)
					);
				}
			}

			for (uint16_t ii = 0; ii < section_size; ++ii) 
			{
				for (uint16_t jj = 0; jj <= section_size; ++jj) 
				{
					section->indices.push_back(section->uniq_indices[ii * (section_size + 1) + jj]);
					section->indices.push_back(section->uniq_indices[(ii + 1) * (section_size + 1) + jj]);
				}
				section->indices.push_back(0u - 1);
			}

			section->ibo = Rainy::CreateBuffer();
			Rainy::IndicesBufferData(section->ibo, section->indices.data(), section->indices.size() * sizeof(uint32_t));
		}
	}

	vbo = Rainy::CreateBuffer();
	Rainy::BufferData(vbo, vertices.data(), vertices.size() * sizeof(VertexLS));
	vao = Rainy::CreateVertexArray();
	Rainy::BufferAttach(vao, vbo, 0, 3, sizeof(VertexLS), (void*)0);
	Rainy::BufferAttach(vao, vbo, 1, 2, sizeof(VertexLS), (void*)offsetof(VertexLS, tex_coord));
	Rainy::BufferAttach(vao, vbo, 2, 3, sizeof(VertexLS), (void*)offsetof(VertexLS, normal));
}

Landscape::Landscape(
	Rainy::Image * image, uint16_t width, uint16_t lenght, uint16_t section_size
)
{
	//if (image->getWidth() != p_width + 1 || image->getHeight() != p_length + 1)
	//	image->rescale(p_width + 1, p_length + 1);

	//float v_height = 0;

	//BYTE *bits = image->getPixels();

	//auto image_width = image->getWidth();
	//auto image_height = image->getHeight();

	//auto pitch = image->getPitch();
	//
	//for (unsigned i = 0; i < image_height; ++i) {
	//	BYTE *pixel = bits;
	//	
	//	for (unsigned j = 0; j < image_width; ++j) {
	//		v_height += pixel[FI_RGBA_RED];
	//		v_height += pixel[FI_RGBA_GREEN];
	//		v_height += pixel[FI_RGBA_BLUE];
	//		pixel += 3;

	//		v_height /= 255 * 3;

	//		VertexLS v;
	//
	//		v.position = { j * side_length, v_height, i * side_length };
	//		vertices.push_back(v);
	//	}
	//	bits += pitch;
	//}

	//// normals (code from LandscapeChanger)

	//for (size_t i = 0; i < length; ++i) {
	//	for (size_t j = 0; j < width; ++j) {
	//		auto& v0 = vertices[i * (width + 1) + j];
	//		auto& v1 = vertices[i * (width + 1) + j + 1];
	//		auto& v2 = vertices[(i + 1) * (width + 1) + j];
	//		auto& v3 = vertices[(i + 1) * (width + 1) + j + 1];

	//		Vector3f normal_0{};
	//		Vector3f normal_1{};

	//		if (((i * (width + 1) + j) / ((width + 1) + 1)) % 2 == 0) {
	//			normal_0 = ((v2.position - v0.position).cross(v1.position - v0.position));
	//			normal_1 = ((v1.position - v3.position).cross(v2.position - v3.position));
	//		}
	//		else {
	//			normal_0 = ((v0.position - v1.position).cross(v3.position - v1.position));
	//			normal_1 = ((v3.position - v2.position).cross(v0.position - v2.position));
	//		}

	//		normal_0.normalize();
	//		normal_1.normalize();

	//		v0.normal += normal_0 + normal_1;
	//		v1.normal += normal_0 + normal_1;
	//		v2.normal += normal_0 + normal_1;
	//		v3.normal += normal_0 + normal_1;
	//	}
	//}

	//for (auto v : vertices)
	//	v.normal.normalize();


	//// faces (code from first constructor)
	//const GLuint vertices_in_line = width + 1;

	//for (GLuint row = 0; row < length; ++row) {
	//	if (row % 2 == 0) {
	//		for (GLuint column = 0; column <= width; ++column) {
	//			faces.push_back(row * vertices_in_line + column);
	//			faces.push_back((row + 1)  * vertices_in_line + column);
	//		}
	//	}
	//	else {
	//		for (int column = width; column > 0; column--) {
	//			faces.push_back((row + 1) * vertices_in_line + column);
	//			faces.push_back(row * vertices_in_line + column - 1);
	//		}
	//	}
	//}
	//GLuint last_vertex_index = length * vertices_in_line;
	//faces.push_back(last_vertex_index);

	//loadToOGL();
}
//
//
void Landscape::updateVBO(LandscapePiece * piece) const
{
	//std::chrono::time_point start_point = std::chrono::high_resolution_clock::now();
	
	for (auto vertex_data : piece->vertices_data) {
		if (vertex_data.first != nullptr) {
			Rainy::BufferSubData(vbo, vertex_data.second * sizeof(VertexLS), sizeof(VertexLS), (void*)(vertices.data() + vertex_data.second));
			/*glBufferSubData(GL_ARRAY_BUFFER,
				vertex_data.second * sizeof(VertexLS), sizeof(VertexLS), (vertices.data() + vertex_data.second));*/
		}
	}

	/*VertexIndexType begin = piece->vertices_data.front().second;
	int difference = 0;

	for (auto vertex_data : piece->vertices_data) {
		difference = begin - vertex_data.second;
		
		if (vertex_data.first == nullptr) {
			if (difference > 0){
				glBufferSubData(GL_ARRAY_BUFFER,
					begin * sizeof(VertexLS), difference * sizeof(VertexLS), (vertices.data() + difference));
			}	
			begin = vertex_data.second;
		}
		else if (difference >= 2) {
			glBufferSubData(GL_ARRAY_BUFFER,
				begin * sizeof(VertexLS), difference * sizeof(VertexLS), (vertices.data() + difference));
			begin = vertex_data.second;
		}

	}*/
	/*
	for (size_t i = 0; i < length; ++i) {
		
		GLuint first_index_in_row = vertex_indices[i * width];
		GLuint last_index_in_row = first_index_in_row + width - 1;

		glBufferSubData(GL_ARRAY_BUFFER, first_index_in_row * sizeof(VertexLS),
			(last_index_in_row - first_index_in_row + 1) * sizeof(VertexLS), (vertices.data() + first_index_in_row));
	}*/
	

	//std::cout << "Time: " << ((std::chrono::high_resolution_clock::now()) - start_point).count() / 1000 << std::endl;
}

void Landscape::draw() const
{
	for (auto section_index : sections_to_draw)
	{
		auto section = sections_map[section_index];
		Rainy::EnableRestartIndex(0u - 1);
		Rainy::IndicesBufferAttach(vao, section->ibo);
		Rainy::DrawElements(Rainy::RenderMode::RN_TRIANGLE_STRIP, vao, section->indices.size());
		Rainy::DisableRestartIndex();
	}
}

bool Landscape::pointInXZRec(float x, float z) const
{
	float topRightXOffset = position.x + (map_width / 2 * section_world_size) - x;
	float topRightZOffset = position.z + (map_lenght / 2 * section_world_size) - z;

	float leftBottomXOffset = position.x - (map_width / 2 * section_world_size) - x;
	float leftBottomZOffset = position.z - (map_lenght / 2 * section_world_size) - z;

	if (leftBottomXOffset > 0 || topRightXOffset < 0 ||
		leftBottomZOffset > 0 || topRightZOffset < 0) return false;

	return true;
}

bool Landscape::getPointOnBorder(
	Vector3f dir, Vector3f point, Vector3f& dest
) const
{
	if (pointInXZRec(point.x, point.z)) {
		dest = point;
		return true;
	}

	Vector3f intersection_point;
	Vector3f min_intersection_point;

	float ray_length;
	float min_ray_length = -1;

	auto point_process = [this, &dest, &ray_length, &min_ray_length, &intersection_point, &min_intersection_point]() {
		if (pointInXZRec(intersection_point.x, intersection_point.z)) 
		{
			if (min_ray_length < 0 || min_ray_length > ray_length) 
			{
				min_ray_length = ray_length;
				dest = intersection_point;
			}
		}
	};

	float x_distance = map_width / 2 * section_world_size;
	float z_distance = map_lenght / 2 * section_world_size;

	// left bottom
	if (InterRayAndSurface({ 0, 0, -z_distance }, { 0, 0, -1 }, point, dir,
		intersection_point, ray_length)) {
		point_process();
	}

	if (InterRayAndSurface({ -x_distance, 0, 0 }, { -1, 0, 0 }, point, dir,
		intersection_point, ray_length)) {
		point_process();
	}

	//right top
	if (InterRayAndSurface({ 0, 0, z_distance }, { 0, 0, 1 }, point, dir,
		intersection_point, ray_length)) {
		point_process();
	}

	if (InterRayAndSurface({ x_distance, 0, 0 }, { 1, 0, 0 }, point, dir,
		intersection_point, ray_length)) {
		point_process();
	}

	if (min_ray_length < 0)
		return false;

	return true;
}

bool Landscape::getSurfacePoint(float x, float z, Vector3f& dest) const
{
	const float CORRECTIVE_VALUE = 0.001f;

	dest = { 0, 0, 0 };
	if (!pointInXZRec(x, z))
		return false;

	float correct_point_x = (x - CORRECTIVE_VALUE);
	float correct_point_z = (z - CORRECTIVE_VALUE);

	int section_x_coord = correct_point_x + map_width / 2;
	int section_z_coord = correct_point_z + map_lenght / 2;
	
	Section * section = sections_map[section_z_coord * map_width + section_x_coord];

	if (section == nullptr) {
		dest = { x, position.y, z };
		return true;
	}

	Vector3f left_bottom_vertex_position{ vertices[section->uniq_indices[0]].position };

	int quad_x_pos = (correct_point_x - left_bottom_vertex_position.x) / vertex_step;
	int quad_z_pos = (correct_point_z - left_bottom_vertex_position.z) / vertex_step;

	VertexLS v0 = vertices[
		section->uniq_indices[quad_z_pos * (section_size + 1) + quad_x_pos]
	];
	VertexLS v1 = vertices[
		section->uniq_indices[(quad_z_pos + 1) * (section_size + 1) + quad_x_pos]
	];
	VertexLS v2 = vertices[
		section->uniq_indices[quad_z_pos * (section_size + 1) + quad_x_pos + 1]
	];

	if ((correct_point_z - quad_z_pos * vertex_step + left_bottom_vertex_position.z) >
		(vertex_step - (correct_point_x - quad_x_pos * vertex_step + left_bottom_vertex_position.x)))
		v0 = vertices[
			section->uniq_indices[(quad_z_pos + 1) * (section_size + 1) + quad_x_pos + 1]
		];

	Vector3f weights = BarycentricCoord(
		{ x, 0, z }, v0.position, v1.position, v2.position
	);

	Vector3f point_on_surface =
		v0.position * weights.x + v1.position * weights.y + v2.position * weights.z;

	dest = point_on_surface;

	return true;
}

bool Landscape::pointBelow(Vector3f point) const
{
	Vector3f point_on_surface;
	if (getSurfacePoint(point.x, point.z, point_on_surface))
		return point.y <= point_on_surface.y;
	return false;
}

bool Landscape::startAboveEndBelow(Vector3f start, Vector3f end) const
{
	return !pointBelow(start) && pointBelow(end);
}

Vector3f Landscape::binarySearch(Vector3f ray_start_point, Vector3f ray,
	float start, float end, int recursion_count) const
{
	/*RN_CORE_TRACE("RecCount: {0}", recursion_count);*/
	float half = start + (end - start) / 2;

	Vector3f ray_half_point = ray_start_point + ray * half;

	if (recursion_count <= 0)
	{
		Vector3f point_os_surface;
		getSurfacePoint(ray_half_point.x, ray_half_point.z, point_os_surface);
		return point_os_surface;
	}

	if (startAboveEndBelow(ray_start_point + ray * start, ray_half_point))
		return binarySearch(ray_start_point, ray, start, half, recursion_count - 1);
	else return binarySearch(ray_start_point, ray, half, end, recursion_count - 1);
}

bool Landscape::rayIntersection(
	Vector3f & dest, Vector3f ray_direction, Vector3f ray_start_point, float ray_distance
) const
{
	constexpr uint8_t PARTS_COUNT = 1000;

	Vector3f start_ray_point;
	getPointOnBorder(ray_direction, ray_start_point, start_ray_point);

	Vector3f end_ray_point;
	getPointOnBorder(ray_direction * (-1.f), ray_start_point + (ray_direction * ray_distance), end_ray_point);

	/*RN_APP_TRACE("Start: {0}, {1}, {2}; End: {3}, {4}, {5};",
		start_ray_point.x, start_ray_point.y, start_ray_point.z, 
		end_ray_point.x, end_ray_point.y, end_ray_point.z);*/

	float ray_length = (end_ray_point - start_ray_point).length() / PARTS_COUNT;

	auto has_section = [this](Vector3f const & point)
	{
		const float CORRECTIVE_VALUE = 0.001f;

		float correct_point_x = (point.x - CORRECTIVE_VALUE);
		float correct_point_z = (point.z - CORRECTIVE_VALUE);

		int section_x_coord = correct_point_x + map_width / 2;
		int section_z_coord = correct_point_z + map_lenght / 2;

		return sections_map[section_z_coord * map_width + section_x_coord] != nullptr;
	};

	if (startAboveEndBelow(start_ray_point, end_ray_point))
	{
		for (uint8_t i = 0; i < PARTS_COUNT; ++i)
		{
			Vector3f start_ray_part_point = start_ray_point + ray_direction * (ray_length * i);
			Vector3f end_ray_part_point = start_ray_point + ray_direction * (ray_length * (i + 1));

			if (startAboveEndBelow(start_ray_part_point, end_ray_part_point) &&
				has_section(start_ray_part_point) && has_section(end_ray_part_point))
			{
				dest = binarySearch(start_ray_part_point, ray_direction, 0, ray_length, 100);
				return true;
			}
		}
	}

	return false;
}

LandscapePiece* Landscape::getTerrainArea(Vector3f const& center, float radius)
{
	std::cout << "Start getTerrainArea" << std::endl;

	LandscapePiece * piece = new LandscapePiece;

	int const half_vertex_rows = (map_lenght * section_size) / 2;
	int const half_vertex_columns = (map_width * section_size) / 2;

	Rainy::RectangleAreai world_vertex_space{
		std::ceilf((half_vertex_columns * vertex_step + center.x - radius) / vertex_step),
		(half_vertex_columns * vertex_step + center.x + radius) / vertex_step,
		std::ceilf((half_vertex_rows * vertex_step + center.z - radius) / vertex_step),
		(half_vertex_rows * vertex_step + center.z + radius) / vertex_step
	};

	if (world_vertex_space.LeftBorder < 0) world_vertex_space.LeftBorder = 0;
	if (world_vertex_space.RightBorder > half_vertex_columns * 2) world_vertex_space.RightBorder = half_vertex_columns * 2;
	if (world_vertex_space.BottomBorder < 0) world_vertex_space.BottomBorder = 0;
	if (world_vertex_space.TopBorder > half_vertex_rows * 2) world_vertex_space.TopBorder = half_vertex_rows * 2;

	//std::cout << center << std::endl;

	std::cout << world_vertex_space.LeftBorder << ", " << world_vertex_space.RightBorder <<
		", " << world_vertex_space.BottomBorder << ", " << world_vertex_space.TopBorder << std::endl;

	piece->length = std::abs(world_vertex_space.TopBorder - world_vertex_space.BottomBorder);
	piece->width = std::abs(world_vertex_space.RightBorder - world_vertex_space.LeftBorder);

	piece->vertices_data.reserve(piece->length * piece->width);

	for (int i = world_vertex_space.BottomBorder; i <= world_vertex_space.TopBorder; ++i) {
		for (int j = world_vertex_space.LeftBorder; j <= world_vertex_space.RightBorder; ++j) {

			Vector2i section_position{
				j / section_size,
				i / section_size
			};
			Section * section = sections_map[section_position.y * map_width + section_position.x];
			if (section == nullptr) {
				if (j % section_size == 0) {
					section_position.x = (j - 1) / section_size;
				}
				if (i % section_size == 0) {
					section_position.y = (i - 1) / section_size;
				}
				section = sections_map[section_position.y * map_width + section_position.x];
				if (section == nullptr) continue;
			}
			Vector2i vertex_position{
				j - section_position.x * section_size,
				i - section_position.y * section_size
			};

			VertexData vertex_data;
			vertex_data.second =
				section->uniq_indices[vertex_position.y * (section_size + 1) + vertex_position.x];
			vertex_data.first = &vertices[vertex_data.second];

			piece->vertices_data.push_back(vertex_data);
		}
	}

	std::cout << "End getTerrainArea" << std::endl;

	return piece;
}

void Landscape::getMaxMinY(float& min_dest, float& max_dest) const
{
	/*min_dest = vertices[0].position.y;
	max_dest = min_dest;
	size_t size = (width + 1) * (length + 1);
	for (size_t i = 1; i < size; ++i)
	{
		float y = vertices[i].position.y;
		if (y < min_dest)
			min_dest = y;
		if (y > max_dest)
			max_dest = y;
	}*/
}

uint8_t* Landscape::getRGBArray(uint16_t bpp) const
{
	/*float y_min = 0;
	float y_max = 0;

	getMaxMinY(y_min, y_max );

	float current_differents_height = y_max - y_min;

	uint16_t vert_in_width = width + 1;
	uint16_t vert_in_height = length + 1;

	uint16_t pitch = ((vert_in_width * bpp) + 31) / 32 * 4;
	uint16_t align = pitch - (vert_in_width * bpp / 8);

	BYTE* bytes = new BYTE[pitch * vert_in_height];

	for (uint16_t i = 0; i < vert_in_height; ++i){
		for (uint16_t j = 0; j < vert_in_width; ++j){
			float y = vertices[i * vert_in_width + j].position.y;
			float div = fabsf(y) / current_differents_height;
			auto color_component = BYTE(div * 255);

			bytes[FI_RGBA_RED] = color_component;
			bytes[FI_RGBA_GREEN] = color_component;
			bytes[FI_RGBA_BLUE] = color_component;
			bytes += 3;
		}
		bytes += align;
	}
	return (bytes - pitch * vert_in_height);*/
	return nullptr;
}

void Landscape::resize(uint32_t new_width, uint32_t new_length, float new_side_length)
{
	/*side_length = new_side_length;

	decltype(vertices) old_data(vertices.size());
	std::copy(vertices.begin(), vertices.end(), old_data.begin());
	vertices.clear();
	faces.clear();

	for (uint16_t i = 0; i < new_length + 1; ++i) {
		for (uint16_t j = 0; j < new_width + 1; ++j) {
			VertexLS new_vertex;
			if (i < (width + 1) && j < (length + 1)) {
				auto old_vertex = old_data[i * (width + 1) + j];
				new_vertex.position = { j * side_length, old_vertex.position.y, i * side_length };
				new_vertex.tex_coord = old_vertex.tex_coord;
				new_vertex.normal = old_vertex.normal;
			}else{
				new_vertex.position = { j * side_length, position.y, i * side_length };
				new_vertex.tex_coord = { 0 };
				new_vertex.normal = { 0, 1, 0 };
			}
			vertices.push_back(new_vertex);
		}
	}

	width = new_width;
	length = new_length;

	const GLuint vertices_in_line = width + 1;
	for (GLuint row = 0; row < length; ++row) {
		if (row % 2 == 0) {
			for (GLuint column = 0; column <= width; ++column) {
				faces.push_back(row * vertices_in_line + column);
				faces.push_back((row + 1)  * vertices_in_line + column);
			}
		}
		else {
			for (int column = width; column > 0; column--) {
				faces.push_back((row + 1) * vertices_in_line + column);
				faces.push_back(row * vertices_in_line + column - 1);
			}
		}
	}
	GLuint last_vertex_index = length * vertices_in_line;
	faces.push_back(last_vertex_index);

	loadToOGL();*/
}

void Landscape::toFlatten()
{
	/*std::cout << "From Flatten" << std::endl;
	float y = position.y;
	std::cout << y << std::endl;
	for (auto & vertex : vertices) {
		vertex.position.y = y;
		vertex.normal = { 0, 1, 0 };
	}
	fullUpdateVBO();*/
}