#include <algorithm>
#include <chrono>

#include "ToolPlus.h"

namespace ls {

	constexpr float MIN_STRENGTH_MOD = 0.25f;
	//BasicTool::BasicTool(Landscape * landscape_ptr)
	//	:landscape_ptr(landscape_ptr) {}
	BasicTool::BasicTool(LandscapePlus * landscape_ptr)
		:m_landscape(landscape_ptr) {}

	//void BasicTool::setLandscape(Landscape * landscape_ptr)
	void BasicTool::SetLandscape(LandscapePlus* landscape_ptr)
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

	void BasicTool::ModifyVertexPositions(SectionPart* section, Brush* brush) {	}

	void BasicTool::ModifyVertexNormals(SectionPart* part)
	{
		auto& vertex_ref = part->Vertices;
		for (size_t i = 0; i < part->Length - 1; ++i) {
			for (size_t j = 0; j < part->Width - 1; ++j) {
				auto v0 = vertex_ref[i * part->Width + j];
				auto v1 = vertex_ref[i * part->Width + j + 1];
				auto v2 = vertex_ref[(i + 1) * part->Width + j];
				auto v3 = vertex_ref[(i + 1) * part->Width + j + 1];

				Vector3f normal_0{};
				Vector3f normal_1{};

				normal_0 = ((v0.Position - v1.Position).cross(v3.Position - v1.Position));
				normal_1 = ((v3.Position - v2.Position).cross(v0.Position - v2.Position));
	
				normal_0.normalize();
				normal_1.normalize();

				v0.Normal += normal_0 + normal_1;
				v1.Normal += normal_0 + normal_1;
				v2.Normal += normal_0 + normal_1;
				v3.Normal += normal_0 + normal_1;
			}
		}

		for (auto& vertex : vertex_ref)
			vertex.Normal.normalize();
	}

	void BasicTool::ModifyLandscape(Brush* brush)
	{
		float radius = brush->size / m_landscape->GetScale();
		auto sections = m_landscape->GetSections(m_lastInterPoint, radius);
		for (SectionPlus* section : sections)
		{
			SectionPart* part = section->GetPart(m_lastInterPoint, radius);
			if (part == nullptr) continue;
			ModifyVertexPositions(part, brush);
			ModifyVertexNormals(part);
			section->ReloadPart(part);
		}
	}

	SculptTool::SculptTool(LandscapePlus  * landscape_ptr, float strength)
		:BasicTool(landscape_ptr), strength(strength) {}

	void SculptTool::ModifyVertexPositions(SectionPart * lp, Brush* brush)
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

		for (VertexPlus& vertex : lp->Vertices) {
			Vector3f &vertex_position = vertex.Position;
			Vector2f xz_position{ vertex_position.x, vertex_position.z };

			float distance = (xz_position - xz_intersection_point).lenght();

			if (distance <= (brush->size / m_landscape->GetScale()))
				vertex_position.y += strength * brush->GetFalloffFactor(distance) * strength_mod;
		}
	}
	
	SmoothTool::SmoothTool(LandscapePlus * landscape_ptr, float strength, uint8_t levels)
		:BasicTool(landscape_ptr), strength(strength), levels(levels) {}
	
	void SmoothTool::ModifyVertexPositions(SectionPart * lp, Brush* brush)
	{
		std::vector<Vector3f> positions;
	
		Vector2f xz_intersection_point{ m_lastInterPoint.x, m_lastInterPoint.z };
	
		for (int i = 0; i < lp->Length; ++i) {
			for (int j = 0; j < lp->Width; ++j) {
				int begin_z = std::clamp<int>(i - levels, 0, lp->Length - 1);
				int end_z = std::clamp<int>(i + levels, 0, lp->Length - 1);
				
				int begin_x = std::clamp<int>(j - levels, 0, lp->Width - 1);
				int end_x = std::clamp<int>(j + levels, 0, lp->Width - 1);
	
				uint16_t count = (end_x - begin_x + 1) * (end_z - begin_z + 1);
				float height_summ = 0;
	
				for (; begin_z <= end_z; ++begin_z) {
					for (; begin_x <= end_x; ++begin_x) {
						height_summ += lp->Vertices[begin_z * lp->Width + begin_x].Position.y;
					}
				}
				
				auto position = lp->Vertices[i * lp->Width + j].Position;
	
				Vector2f xz_position{ position.x, position.z };
	
				float distance = (xz_position - xz_intersection_point).lenght();
	
				float height_increment = (position.y - (height_summ / count))  * strength * brush->GetFalloffFactor(distance);
				position.y -= height_increment;
	
				positions.push_back(position);
				/*std::cout << "height_increment: " << height_increment
					<< " height_summ: " << height_summ
					<< " count: " << count << std::endl;*/
			}
		}
	
		for (size_t i = 0; i < lp->Vertices.size(); ++i)
			lp->Vertices[i].Position = positions[i];
	}
}