#include <algorithm>
#include <chrono>

#include "ToolPlus.h"

namespace ls {

	constexpr float MIN_STRENGTH_MOD = 0.25f;
	//BasicTool::BasicTool(Landscape * landscape_ptr)
	//	:landscape_ptr(landscape_ptr) {}
	BasicTool::BasicTool(Landscape * landscape_ptr)
		:m_landscape(landscape_ptr) {}

	//void BasicTool::setLandscape(Landscape * landscape_ptr)
	void BasicTool::SetLandscape(Landscape* landscape_ptr)
	{
		RN_ASSERT(landscape_ptr != nullptr, "Assert in SetLandscape ptr == nullptr");
		this->m_landscape = landscape_ptr;
	}

	void BasicTool::UpdateLastIntersectionPoint(Vector3f point)
	{
		//if (m_landscape != nullptr) 
		//{
		//	bool result = m_landscape->RayIntersection(
		//		m_lastInterPoint, ray.Direction, ray.Start, ray.Length);
		//	//std::cout << last_intersection_point << std::endl;
		//}
		m_lastInterPoint = point;
	}

	void BasicTool::ModifyVertexPositions(LandscapePart* lp, Brush* brush) {}

	void BasicTool::ModifyLandscape(Brush* brush)
	{
		if (m_landscape != nullptr)
		{
			LandscapePart* lp = m_landscape->GetPart(m_lastInterPoint, brush->size);
			ModifyVertexPositions(lp, brush);
			ModifyVertexNormals(lp);
			m_landscape->RealoadPart(lp);
		}
		//std::cout << "Time: " << ((std::chrono::high_resolution_clock::now()) - start_point).count() / 1000 << std::endl;
	}

	void BasicTool::ModifyVertexNormals(LandscapePart * lp)
	{
		uint32_t negYNormals = 0;
		uint32_t posYNormals = 0;

		auto NormalCalc = [&negYNormals, &posYNormals](Vector3f v0, Vector3f v1, Vector3f v2)
		{
			Vector3f a = v1 - v0;
			Vector3f b = v2 - v0;
			Vector3f crossResult = a.cross(b);
			if (crossResult.y < 0) negYNormals++;
			else posYNormals++;
			return crossResult;
		};

		const uint32_t vertexInWidth = lp->Width + 1;
		auto& vertex_ref = lp->VerticesData;
		for (size_t z = 1; z < lp->Length; ++z) 
		{
			for (size_t x = 1; x < lp->Width; ++x) 
			{
				Vertex* center = vertex_ref[z * vertexInWidth + x].first;
				if (center == nullptr) continue;

				Vertex* const& right = vertex_ref[z * vertexInWidth + x + 1].first;
				Vertex* const& left = vertex_ref[z * vertexInWidth + x - 1].first;
				Vertex* const& top = vertex_ref[(z + 1) * vertexInWidth + x].first;
				Vertex* const& bottom = vertex_ref[(z - 1) * vertexInWidth + x].first;
				Vertex* const& topLeft = vertex_ref[(z + 1) * vertexInWidth + x - 1].first;
				Vertex* const& bottomRight = vertex_ref[(z - 1) * vertexInWidth + x + 1].first;

				Vector3f sum = { 0, 0, 0 };
				if (topLeft != nullptr)
				{
					if (left != nullptr) sum += NormalCalc(left->Position, topLeft->Position, center->Position);
					if (top != nullptr) sum += NormalCalc(top->Position, topLeft->Position, center->Position) * -1.f;
				}

				if (bottomRight != nullptr)
				{
					if (bottom != nullptr) sum += NormalCalc(bottom->Position, center->Position, bottomRight->Position);
					if (right != nullptr) sum += NormalCalc(right->Position, center->Position, bottomRight->Position) * -1.f;
				}

				if (top != nullptr && right != nullptr) 
					sum += NormalCalc(center->Position, top->Position, right->Position);
				if (bottom != nullptr && left != nullptr)
					sum += NormalCalc(center->Position, left->Position, bottom->Position) * -1.f;

				sum.normalize();
				center->Normal = sum;

				/*Vector3f normal_0{};
				Vector3f normal_1{};

				if (((i * lp.Width + j) / (m_landscape->GetWidth() + 1)) % 2 == 0) {
					normal_0 = ((v2->position - v0->position).cross(v1->position - v0->position));
					normal_1 = ((v1->position - v3->position).cross(v2->position - v3->position));
				}
				else {*/
				//	normal_0 = ((v0->Position - v1->Position).cross(v3->Position - v1->Position));
				//	normal_1 = ((v3->Position - v2->Position).cross(v0->Position - v2->Position));
				////}

				//normal_0.normalize();
				//normal_1.normalize();

				//v0->Normal += normal_0 + normal_1;
				//v1->Normal += normal_0 + normal_1;
				//v2->Normal += normal_0 + normal_1;
				//v3->Normal += normal_0 + normal_1;
			}
		}

		

		/*for (auto index : vertex_ref)
			index.first->Normal.normalize();*/
	}

	SculptTool::SculptTool(Landscape * landscape_ptr, float strength)
		:BasicTool(landscape_ptr), strength(strength) {}

	void SculptTool::ModifyVertexPositions(LandscapePart* lp, Brush* brush)
	{
		static std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
		static auto p_last_inter_point = m_lastInterPoint;
		static float strength_mod = 0;
		if (p_last_inter_point == m_lastInterPoint) {
			strength_mod =
				(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count()) / 2000.f;
		}
		else {
			strength_mod = MIN_STRENGTH_MOD;
			time = std::chrono::steady_clock::now();
			p_last_inter_point = m_lastInterPoint;
		}

		strength_mod = std::clamp<float>(strength_mod, MIN_STRENGTH_MOD, 1.f);

		Vector2f xz_intersection_point{ m_lastInterPoint.x, m_lastInterPoint.z };

		for (auto vertex_data : lp->VerticesData) {
			if (vertex_data.first == nullptr || vertex_data.second == (0u - 1)) continue;
			Vector3f &vertex_position = vertex_data.first->Position;
			Vector2f xz_position{ vertex_position.x, vertex_position.z };

			float distance = (xz_position - xz_intersection_point).lenght();

			if (distance <= brush->size)
				vertex_position.y += strength * brush->GetFalloffFactor(distance) * strength_mod;
		}
	}
	
	SmoothTool::SmoothTool(Landscape * landscape_ptr, float strength, uint8_t levels)
		:BasicTool(landscape_ptr), strength(strength), levels(levels) {}
	
	void SmoothTool::ModifyVertexPositions(LandscapePart* lp, Brush* brush)
	{
		std::vector<Vector3f> positions;
	
		Vector2f xzIntersectionPoint{ m_lastInterPoint.x, m_lastInterPoint.z };
	
		uint32_t vertexInWidth = lp->Width + 1;

		for (int z = 0; z <= lp->Length; z++) {
			for (int x = 0; x <= lp->Width; x++) {
				int zBegin = std::clamp<int>(z - levels, 0, lp->Length - 1);
				int zEnd = std::clamp<int>(z + levels, 0, lp->Length - 1);
				
				int xBegin = std::clamp<int>(x - levels, 0, lp->Width - 1);
				int xEnd = std::clamp<int>(x + levels, 0, lp->Width - 1);
	
				uint16_t count = (xEnd - xBegin + 1) * (zEnd - zBegin + 1);
				float heightSum = 0;

				Vertex* centerVertex = lp->VerticesData[z * vertexInWidth + x].first;
				if (centerVertex == nullptr)
				{
					positions.push_back({});
					continue;
				}
				auto position = centerVertex->Position;

				for (; zBegin <= zEnd; ++zBegin) {
					for (; xBegin <= xEnd; ++xBegin) {
						Vertex* vertexPtr = lp->VerticesData[zBegin * vertexInWidth + xBegin].first;
						if (vertexPtr != nullptr) heightSum += vertexPtr->Position.y;
					}
				}
	
				Vector2f xzPosition{ position.x, position.z };
	
				float distance = (xzPosition - xzIntersectionPoint).lenght();
	
				float height_increment = (position.y - (heightSum / count))  * strength * brush->GetFalloffFactor(distance);
				position.y -= height_increment;
	
				positions.push_back(position);
			}
		}
	
		for (size_t i = 0; i < lp->VerticesData.size(); ++i)
		{
			Vertex* vertexPtr = lp->VerticesData[i].first;
			if (vertexPtr != nullptr) vertexPtr->Position = positions[i];
		}
	}
}	