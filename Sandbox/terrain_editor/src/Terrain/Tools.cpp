#include <algorithm>
#include <chrono>

#include "Tools.h"

namespace te {
	//
	//void BasicToolCommand::ModifyTerrain(Terrain* terrain, Vector3f interPoint)
	//{
	//	if (terrain != nullptr)
	//	{
	//		TerrainArea* area = terrain->GetArea({ interPoint.x, interPoint.z }, m_brush.size);
	//		ModifyPositions(area, interPoint);
	//		ModifyNormals(area);
	//		terrain->ApplyChanges(area);
	//		delete area;
	//	}
	//	//std::cout << "Time: " << ((std::chrono::high_resolution_clock::now()) - start_point).count() / 1000 << std::endl;
	//}

	//void BasicToolCommand::RecalcNormals(TerrainArea* area)
	//{
	//	uint32_t negYNormals = 0;
	//	uint32_t posYNormals = 0;

	//	auto NormalCalc = [&negYNormals, &posYNormals](Vector3f v0, Vector3f v1, Vector3f v2)
	//	{
	//		Vector3f a = v1 - v0;
	//		Vector3f b = v2 - v0;
	//		Vector3f crossResult = a.cross(b);
	//		if (crossResult.y < 0) negYNormals++;
	//		else posYNormals++;
	//		return crossResult;
	//	};

	//	//auto& vertex_ref = lp->VerticesData;
	//	for (size_t z = 1; z < area->Height - 1; ++z)
	//	{
	//		for (size_t x = 1; x < area->Width - 1; ++x) 
	//		{
	//			uint32_t centerIndex = area->Map[z * area->Width + x];
	//			if (centerIndex == TerrainArea::INCORRECT_INDEX) continue;
	//			Vector3f center = area->Positions[centerIndex];

	//			uint32_t rightIndex = area->Map[z * area->Width + x + 1],
	//				leftIndex = area->Map[z * area->Width + x - 1],
	//				topIndex = area->Map[(z + 1) * area->Width + x],
	//				bottomIndex = area->Map[(z - 1) * area->Width + x],
	//				topLeftIndex = area->Map[(z + 1) * area->Width + x - 1],
	//				bottomRightIndex = area->Map[(z - 1) * area->Width + x + 1];

	//			Vector3f sum = { 0, 0, 0 };
	//			if (topLeftIndex != TerrainArea::INCORRECT_INDEX)
	//			{
	//				if (leftIndex != TerrainArea::INCORRECT_INDEX) sum += NormalCalc(area->Positions[leftIndex], area->Positions[topLeftIndex], center);
	//				if (topIndex != TerrainArea::INCORRECT_INDEX) sum += NormalCalc(area->Positions[topIndex], area->Positions[topLeftIndex], center) * -1.f;
	//			}

	//			if (bottomRightIndex != TerrainArea::INCORRECT_INDEX)
	//			{
	//				if (bottomIndex != TerrainArea::INCORRECT_INDEX) sum += NormalCalc(area->Positions[bottomIndex], center, area->Positions[bottomRightIndex]);
	//				if (rightIndex != TerrainArea::INCORRECT_INDEX) sum += NormalCalc(area->Positions[rightIndex], center, area->Positions[bottomRightIndex]) * -1.f;
	//			}

	//			if (topIndex != TerrainArea::INCORRECT_INDEX && rightIndex != TerrainArea::INCORRECT_INDEX)
	//				sum += NormalCalc(center, area->Positions[topIndex], area->Positions[rightIndex]);
	//			if (bottomIndex != TerrainArea::INCORRECT_INDEX && leftIndex != TerrainArea::INCORRECT_INDEX)
	//				sum += NormalCalc(center, area->Positions[leftIndex], area->Positions[bottomIndex]) * -1.f;

	//			sum.normalize();
	//			area->Normals[centerIndex] = { sum.x, sum.z, sum.y };
	//		}
	//	}

	//	
	//}

	void ScupltToolCommand::ModifyPositions(TerrainArea* area, Vector3f interPoint)
	{
		Vector2f xz_intersection_point{ interPoint.x, interPoint.z };

		for (auto dataIndex : area->Map) {
			if (dataIndex == area->INCORRECT_INDEX) continue;
			Vector3f& vertex_position = area->Positions[dataIndex];
			Vector2f xz_position{ vertex_position.x, vertex_position.z };

			float distance = (xz_position - xz_intersection_point).lenght();

			if (distance <= m_brush.size)
			{
				float increment = m_strength * m_brush.GetFalloffFactor(distance);
				vertex_position.y += increment;
			}
		}
	}
		
	void SmoothToolCommand::ModifyPositions(TerrainArea* area, Vector3f interPoint)
	{
		std::vector<Vector3f> positions;
	
		Vector2f xzIntersectionPoint{ interPoint.x, interPoint.z };
	
		uint32_t vertexInWidth = area->Width;

		for (int z = 0; z < area->Height; z++) {
			for (int x = 0; x < area->Width; x++) {
				int zBegin = std::clamp<int>(z - m_levels, 0, area->Height - 1);
				int zEnd = std::clamp<int>(z + m_levels, 0, area->Height - 1);
				
				int xBegin = std::clamp<int>(x - m_levels, 0, area->Width - 1);
				int xEnd = std::clamp<int>(x + m_levels, 0, area->Width - 1);
	
				uint16_t count = (xEnd - xBegin + 1) * (zEnd - zBegin + 1);
				float heightSum = 0;

				uint32_t dataIndex = area->Map[z * vertexInWidth + x];
				if (dataIndex == TerrainArea::INCORRECT_INDEX)
				{
					positions.push_back({});
					continue;
				}
				auto position = area->Positions[dataIndex];

				for (; zBegin <= zEnd; zBegin++) 
				{
					for (; xBegin <= xEnd; xBegin++) 
					{
						uint32_t index = area->Map[zBegin * vertexInWidth + xBegin];
						if (index != TerrainArea::INCORRECT_INDEX) heightSum += area->Positions[index].y;
					}
				}
	
				Vector2f xzPosition{ position.x, position.z };
	
				float distance = (xzPosition - xzIntersectionPoint).lenght();
	
				float height_increment = (position.y - (heightSum / count)) * m_strength * m_brush.GetFalloffFactor(distance);
				position.y -= height_increment;
				if (position.y < 0) position.y = 0;

				positions.push_back(position);
			}
		}
	
		for (size_t i = 0; i < area->Positions.size(); ++i)
		{
			area->Positions[i] = positions[i];
		}
	}

}	