#include <cmath>

#include "Rainy/Rainy.h"
#include "Rainy/Render/Renderer.h"

#include "NewLandscape.h"

NewLandscape::NewLandscape(
	Vector3f position, uint16_t width, uint16_t lenght, uint16_t section_size, float section_world_size
)
	:m_position(position), section_size(section_size), section_world_size(section_world_size),
	vertex_step(section_world_size / section_size)
{
	uint32_t const vertices_rows = lenght * section_size + 1;
	uint32_t const vertices_columns = width * section_size + 1;

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
			NewLandscapeSection * section = new NewLandscapeSection();
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

			//section->ibo = Rainy::CreateBuffer();
			//Rainy::IndicesBufferData(section->ibo, section->indices.data(), section->indices.size() * sizeof(uint32_t));
			section->indexBuffer = IndexBuffer::Create(section->indices.size() * sizeof(uint32_t), 
				section->indices.size(), section->indices.data());
		}
	}

	vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(NewVertexLS), vertices.data());
	auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false), 
		Rainy::BufferElement(Rainy::FLOAT2, false), Rainy::BufferElement(Rainy::FLOAT3, false) };
	vertexBuffer->SetBufferLayout({ elements, vertices.size() });
	vertexArray = VertexArray::Create();
	vertexArray->SetVertexBuffer(vertexBuffer);
	/*vbo = Rainy::CreateBuffer();
	Rainy::BufferData(vbo, vertices.data(), vertices.size() * sizeof(NewVertexLS));
	vao = Rainy::CreateVertexArray();
	Rainy::BufferAttach(vao, vbo, 0, 3, sizeof(NewVertexLS), (void*)0);
	Rainy::BufferAttach(vao, vbo, 1, 2, sizeof(NewVertexLS), (void*)offsetof(NewVertexLS, tex_coord));
	Rainy::BufferAttach(vao, vbo, 2, 3, sizeof(NewVertexLS), (void*)offsetof(NewVertexLS, normal));*/
}

void NewLandscape::Draw(Rainy::Shader* shader) const
{
	for (auto section_index : sections_to_draw)
	{
		auto section = sections_map[section_index];
		Rainy::EnableRestartIndex(0u - 1);
		//Rainy::IndicesBufferAttach(vao, section->ibo);
		vertexArray->SetIndexBuffer(section->indexBuffer);
		if (section->selected) 
			shader->SetUniformBool("selected", true);
		else shader->SetUniformBool("selected", false);
		//Rainy::DrawElements(Rainy::RenderMode::RN_TRIANGLE_STRIP, vao, section->indices.size());
		Rainy::DrawVertexArray(Rainy::RenderMode::RN_TRIANGLE_STRIP, vertexArray);
		Rainy::DisableRestartIndex();
	}
}

bool NewLandscape::RayIntersection(
	Vector3f & dest, Vector3f ray_direction, Vector3f ray_start_point, float ray_distance
) const
{
	constexpr uint8_t PARTS_COUNT = 1000;

	Vector3f start_ray_point;
	GetPointOnBorder(ray_direction, ray_start_point, start_ray_point);

	Vector3f end_ray_point;
	GetPointOnBorder(ray_direction * (-1.f), ray_start_point + (ray_direction * ray_distance), end_ray_point);

	RN_APP_TRACE("Start: {0}, {1}, {2}; End: {3}, {4}, {5};",
		start_ray_point.x, start_ray_point.y, start_ray_point.z,
		end_ray_point.x, end_ray_point.y, end_ray_point.z);

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

	if (StartAboveEndBelow(start_ray_point, end_ray_point))
	{
		for (uint8_t i = 0; i < PARTS_COUNT; ++i)
		{
			Vector3f start_ray_part_point = start_ray_point + ray_direction * (ray_length * i);
			Vector3f end_ray_part_point = start_ray_point + ray_direction * (ray_length * (i + 1));

			/*if (StartAboveEndBelow(start_ray_part_point, end_ray_part_point) &&
				has_section(start_ray_part_point) && has_section(end_ray_part_point))*/
			if (StartAboveEndBelow(start_ray_part_point, end_ray_part_point))
			{
				dest = BinarySearch(start_ray_part_point, ray_direction, 0, ray_length, 100);
				return has_section(start_ray_part_point) && has_section(end_ray_part_point);
			}
		}
	}

	return false;
}

NewLandscapeSection* NewLandscape::GetSection(Vector3f position) const
{
	uint32_t x = (position.x + map_width * section_world_size / 2) / section_world_size;
	uint32_t z = (position.z + map_lenght * section_world_size / 2) / section_world_size;

	uint32_t sectionIndex = z * map_width + x;

	if (sectionIndex >= sections_map.size())
		return nullptr;

	return sections_map[z * map_width + x];
}

void NewLandscape::AddSection(Vector3f point)
{
	int x = (point.x + map_width * section_world_size / 2) / section_world_size;
	int z = (point.z + map_lenght * section_world_size / 2) / section_world_size;
	
	static const uint32_t vericesCount = section_size + 1;
	uint32_t xStart = 0;
	uint32_t xEnd = vericesCount;
	uint32_t zStart = 0;
	uint32_t zEnd = vericesCount;

	uint32_t sectionIndex = z * map_width + x;
	if (sectionIndex >= sections_map.size()) IncreaseMapSize();

	if (x - 1 >= 0)
	{
		sectionIndex = z * map_width + x - 1;
		if (sections_map[sectionIndex] != nullptr)
		{
			xStart += 1;
		}
	}
	if (x + 1 < map_width)
	{
		sectionIndex = z * map_width + x + 1;
		if (sections_map[sectionIndex] != nullptr)
		{
			xEnd -= 1;
		}
	}
	if (z - 1 >= 0)
	{
		sectionIndex = (z - 1) * map_width + x;
		if (sections_map[sectionIndex] != nullptr)
		{
			zStart += 1;
		}
	}
	if (z + 1 < map_lenght)
	{
		sectionIndex = (z + 1) * map_width + x;
		if (sections_map[sectionIndex] != nullptr)
		{
			zEnd -= 1;
		}
	}

	// if not neighbor then exit 
	if (xStart == 0 && zStart == 0 && xEnd == vericesCount && zEnd == vericesCount) return;

	int xOffset = std::floorf(point.x / section_world_size);
	int zOffset = std::floorf(point.z / section_world_size);
	Vector3f offset = { float(xOffset), 0, float(zOffset) };

	uint32_t prevSize = vertices.size();

	for (uint32_t z = zStart; z < zEnd; ++z) {
		for (uint32_t x = xStart; x < xEnd; ++x) {
			vertices.push_back({
				Vector3f{ x * vertex_step, 0, z * vertex_step } + offset,
				{ 0, 1, 0 },
				{ 0, 0 }
			});
		}
	}

	NewLandscapeSection * section = new NewLandscapeSection();
	sectionIndex = z * map_width + x;
	sections_map[sectionIndex] = section;
	sections_to_draw.push_back(sectionIndex);

	section->uniq_indices.resize(vericesCount * vericesCount);

	uint32_t i = 0;
	for (uint32_t z = zStart; z < zEnd; ++z) 
	{
		for (uint32_t x = xStart; x < xEnd; ++x, ++i) 
		{
			section->uniq_indices[z * vericesCount + x] = prevSize + i;
		}
	}

	if (xStart == 1)
	{
		NewLandscapeSection* neighborSection = sections_map[z * map_width + x - 1];
		for (uint32_t z = 0; z < vericesCount; ++z)
		{
			section->uniq_indices[z * vericesCount] = 
				neighborSection->uniq_indices[z * vericesCount + vericesCount - 1];
		}
	}

	if (xEnd == vericesCount - 1)
	{
		NewLandscapeSection* neighborSection = sections_map[z * map_width + x + 1];
		for (uint32_t z = 0; z < vericesCount; ++z)
		{
			section->uniq_indices[z * vericesCount + vericesCount - 1] =
				neighborSection->uniq_indices[z * vericesCount];
		}
	}
	if (zStart == 1)
	{
		NewLandscapeSection* neighborSection = sections_map[(z - 1) * map_width + x];
		for (uint32_t x = 0; x < vericesCount; ++x)
		{
			section->uniq_indices[x] =	
				neighborSection->uniq_indices[(vericesCount - 1) * vericesCount + x];
		}
	}
	if (zEnd == vericesCount - 1)
	{
		NewLandscapeSection* neighborSection = sections_map[(z + 1) * map_width + x];
		for (uint32_t x = 0; x < vericesCount; ++x)
		{
			section->uniq_indices[(vericesCount - 1) * vericesCount + x] =
				neighborSection->uniq_indices[x];
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

	/*section->ibo = Rainy::CreateBuffer();
	Rainy::IndicesBufferData(section->ibo, section->indices.data(), section->indices.size() * sizeof(uint32_t));
	*/
	section->indexBuffer = IndexBuffer::Create(section->indices.size() * sizeof(uint32_t),
		section->indices.size(), section->indices.data());

	ReloadVertexBuffer();

	if (x == 0 || z == 0 || x == (map_width - 1) || z == (map_lenght - 1)) 
		IncreaseMapSize();
}

NewLandscapePiece* NewLandscape::GetTerrainArea(Vector3f const& center, float radius)
{
	RN_APP_INFO("Start getTerrainArea");

	NewLandscapePiece * piece = new NewLandscapePiece;

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

	RN_APP_TRACE("WorldVertexSpace: l{0}, r{1}, t{2}, b{3}", 
		world_vertex_space.LeftBorder, world_vertex_space.RightBorder, 
		world_vertex_space.TopBorder, world_vertex_space.BottomBorder);

	
	piece->length = std::abs(world_vertex_space.TopBorder - world_vertex_space.BottomBorder);
	piece->width = std::abs(world_vertex_space.RightBorder - world_vertex_space.LeftBorder);

	piece->vertices_data.reserve(piece->length * piece->width);

	for (int i = world_vertex_space.BottomBorder; i <= world_vertex_space.TopBorder; ++i) {
		for (int j = world_vertex_space.LeftBorder; j <= world_vertex_space.RightBorder; ++j) {

			Vector2i section_position{
				j / section_size,
				i / section_size
			};
			NewLandscapeSection * section = sections_map[section_position.y * map_width + section_position.x];
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

			NewVertexData vertex_data;
			vertex_data.second =
				section->uniq_indices[vertex_position.y * (section_size + 1) + vertex_position.x];
			vertex_data.first = &vertices[vertex_data.second];

			piece->vertices_data.push_back(vertex_data);
		}
	}

	RN_APP_INFO("End getTerrainArea");

	return piece;
}

void NewLandscape::UpdateVertexBuffer(NewLandscapePiece * piece)
{
	for (auto vertex_data : piece->vertices_data) {
		if (vertex_data.first != nullptr) {
			vertexBuffer->SubData(vertex_data.second * sizeof(NewVertexLS), 
				sizeof(NewVertexLS), (void*)(vertices.data() + vertex_data.second));
			/*Rainy::BufferSubData(vbo, vertex_data.second * sizeof(NewVertexLS), 
				sizeof(NewVertexLS), (void*)(vertices.data() + vertex_data.second));
*/
		}
	}
}

bool NewLandscape::PointInXZRec(float x, float z) const
{
	float topRightXOffset = m_position.x + (map_width / 2 * section_world_size) - x;
	float topRightZOffset = m_position.z + (map_lenght / 2 * section_world_size) - z;

	float leftBottomXOffset = m_position.x - (map_width / 2 * section_world_size) - x;
	float leftBottomZOffset = m_position.z - (map_lenght / 2 * section_world_size) - z;

	if (leftBottomXOffset > 0 || topRightXOffset < 0 ||
		leftBottomZOffset > 0 || topRightZOffset < 0) return false;

	return true;
}

bool NewLandscape::GetPointOnBorder(Vector3f dir, Vector3f point, Vector3f& dest) const
{
	if (PointInXZRec(point.x, point.z)) 
	{
		dest = point;
		return true;
	}

	Vector3f intersection_point;
	Vector3f min_intersection_point;

	float ray_length;
	float min_ray_length = -1;

	auto point_process = [this, &dest, &ray_length, &min_ray_length, &intersection_point, &min_intersection_point]() {
		if (PointInXZRec(intersection_point.x, intersection_point.z))
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

bool NewLandscape::GetSurfacePoint(float x, float z, Vector3f& dest) const
{
	const float CORRECTIVE_VALUE = 0.001f;

	dest = { 0, 0, 0 };
	if (!PointInXZRec(x, z))
		return false;

	float correct_point_x = (x - CORRECTIVE_VALUE);
	float correct_point_z = (z - CORRECTIVE_VALUE);

	int section_x_coord = correct_point_x + map_width / 2;
	int section_z_coord = correct_point_z + map_lenght / 2;

	NewLandscapeSection* section = sections_map[section_z_coord * map_width + section_x_coord];

	if (section == nullptr) {
		dest = { x, m_position.y, z };
		return true;
	}

	Vector3f left_bottom_vertex_position{ vertices[section->uniq_indices[0]].position };

	int quad_x_pos = (correct_point_x - left_bottom_vertex_position.x) / vertex_step;
	int quad_z_pos = (correct_point_z - left_bottom_vertex_position.z) / vertex_step;

	RN_ASSERT(quad_x_pos >= 0 && quad_z_pos >= 0, "Error in GetSurfacePoint : quad_x_pos {0}, quad_z_pos{1}", quad_x_pos, quad_z_pos);

	NewVertexLS v0 = vertices[
		section->uniq_indices[quad_z_pos * (section_size + 1) + quad_x_pos]
	];
	NewVertexLS v1 = vertices[
		section->uniq_indices[(quad_z_pos + 1) * (section_size + 1) + quad_x_pos]
	];
	NewVertexLS v2 = vertices[
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

bool NewLandscape::PointBelow(Vector3f point) const
{
	Vector3f point_on_surface;
	if (GetSurfacePoint(point.x, point.z, point_on_surface))
		return point.y <= point_on_surface.y;
	return false;
}

bool NewLandscape::StartAboveEndBelow(Vector3f start, Vector3f end) const
{
	return !PointBelow(start) && PointBelow(end);
}

Vector3f NewLandscape::BinarySearch(Vector3f ray_start_point, Vector3f ray,
	float start, float end, int recursion_count) const
{
	/*RN_CORE_TRACE("RecCount: {0}", recursion_count);*/
	float half = start + (end - start) / 2;

	Vector3f ray_half_point = ray_start_point + ray * half;

	if (recursion_count <= 0)
	{
		Vector3f point_os_surface;
		GetSurfacePoint(ray_half_point.x, ray_half_point.z, point_os_surface);
		return point_os_surface;
	}

	if (StartAboveEndBelow(ray_start_point + ray * start, ray_half_point))
		return BinarySearch(ray_start_point, ray, start, half, recursion_count - 1);
	else return BinarySearch(ray_start_point, ray, half, end, recursion_count - 1);
}

void NewLandscape::IncreaseMapSize()
{
	uint32_t prevWidth = map_width;
	uint32_t prevLength = map_lenght;
	map_lenght *= 2;
	map_width *= 2;
	std::vector<NewLandscapeSection*> prevMap = sections_map;
	sections_map.clear();
	sections_map.resize(map_lenght * map_width, nullptr);
	sections_to_draw.clear();

	uint32_t xOffset = (map_width - prevWidth) / 2;
	uint32_t zOffset = (map_lenght - prevLength) / 2;

	for (uint32_t z = 0; z < prevLength; z++)
	{
		for (uint32_t x = 0; x < prevWidth; x++)
		{
			uint32_t newIndex = (z + zOffset) * map_width + x + xOffset;
			sections_map[newIndex] = prevMap[z * prevWidth + x];
			if(prevMap[z * prevWidth + x] != nullptr)
				sections_to_draw.push_back(newIndex);
		}
	}
}

void NewLandscape::ReloadVertexBuffer()
{
	delete vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(NewVertexLS), vertices.data());
	auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false),
		Rainy::BufferElement(Rainy::FLOAT2, false), Rainy::BufferElement(Rainy::FLOAT3, false) };
	vertexBuffer->SetBufferLayout({ elements, vertices.size() });
	vertexArray->SetVertexBuffer(vertexBuffer);
	/*Rainy::DeleteBuffer(vbo);
	vbo = Rainy::CreateBuffer();
	Rainy::BufferData(vbo, vertices.data(), vertices.size() * sizeof(NewVertexLS));
	Rainy::BufferAttach(vao, vbo, 0, 3, sizeof(NewVertexLS), (void*)0);
	Rainy::BufferAttach(vao, vbo, 1, 2, sizeof(NewVertexLS), (void*)offsetof(NewVertexLS, tex_coord));
	Rainy::BufferAttach(vao, vbo, 2, 3, sizeof(NewVertexLS), (void*)offsetof(NewVertexLS, normal));*/
}
