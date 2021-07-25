#include <algorithm>
#include <chrono>

#include "Tool.h"
#include "../Config.h"


//BasicTool::BasicTool(Landscape * landscape_ptr)
//	:landscape_ptr(landscape_ptr) {}
BasicTool::BasicTool(NewLandscape * landscape_ptr)
	:landscape_ptr(landscape_ptr) {}


//void BasicTool::setLandscape(Landscape * landscape_ptr)
void BasicTool::setLandscape(NewLandscape * landscape_ptr)
{
	this->landscape_ptr = landscape_ptr;
}

void BasicTool::updateLastIntersectionPoint(IntersectionRay const & ray)
{
	if (landscape_ptr != nullptr) {
		bool result = landscape_ptr->RayIntersection(
			last_intersection_point, ray.direction, ray.start_position, ray.length);
		//std::cout << last_intersection_point << std::endl;
	}
}

void BasicTool::updatePositions(NewLandscapePiece * lp, Brush const& brush)	{	}

void BasicTool::updateNormals(NewLandscapePiece * lp)
{
	/*for (size_t i = 0; i < lp->length - 1; ++i) {
		for (size_t j = 0; j < lp->width - 1; ++j) {
			auto v0 = vertex_ref[i * lp->width + j];
			auto v1 = vertex_ref[i * lp->width + j + 1];
			auto v2 = vertex_ref[(i + 1) * lp->width + j];
			auto v3 = vertex_ref[(i + 1) * lp->width + j + 1];

			Vector3f normal_0{};
			Vector3f normal_1{};

			if (((i * lp.width + j) / (landscape_ptr->getWidth() + 1)) % 2 == 0) {
				normal_0 = ((v2->position - v0->position).cross(v1->position - v0->position));
				normal_1 = ((v1->position - v3->position).cross(v2->position - v3->position));
			}
			else {
				normal_0 = ((v0->position - v1->position).cross(v3->position - v1->position));
				normal_1 = ((v3->position - v2->position).cross(v0->position - v2->position));
			}

			normal_0.normalize();
			normal_1.normalize();

			v0->normal += normal_0 + normal_1;
			v1->normal += normal_0 + normal_1;
			v2->normal += normal_0 + normal_1;
			v3->normal += normal_0 + normal_1;
		}
	}
	for (auto index : vertex_ref)
		index->normal.normalize();*/
}

void BasicTool::update(Brush const & brush)
{
	
	if (landscape_ptr != nullptr) 
	{
		NewLandscapePiece * lp = landscape_ptr->GetTerrainArea(last_intersection_point, brush.size);
		updatePositions(lp, brush);
		updateNormals(lp);
		landscape_ptr->UpdateVertexBuffer(lp);
	}
	
	
	//std::cout << "Time: " << ((std::chrono::high_resolution_clock::now()) - start_point).count() / 1000 << std::endl;
}

SculptTool::SculptTool(NewLandscape * landscape_ptr, float strength)
	:BasicTool(landscape_ptr), strength(strength){}

void SculptTool::updatePositions(NewLandscapePiece * lp, Brush const& brush)
{
	static std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
	static auto p_last_inter_point = last_intersection_point;
	static float strength_mod = 0;
	if (p_last_inter_point == last_intersection_point) {
		strength_mod = 
			(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count()) / 2000.f;
	}
	else {
		strength_mod = MIN_STRENGTH_MOD;
		time = std::chrono::steady_clock::now();
		p_last_inter_point = last_intersection_point;
	}

	strength_mod = std::clamp<float>(strength_mod, MIN_STRENGTH_MOD, 1.f);

	Vector2f xz_intersection_point{ last_intersection_point.x, last_intersection_point.z };

	for (auto vertex_data : lp->vertices_data) {
		Vector3f &vertex_position = vertex_data.first->position;
		Vector2f xz_position{ vertex_position.x, vertex_position.z };

		float distance = (xz_position - xz_intersection_point).lenght();

		if (distance <= brush.size)
			vertex_position.y += strength * brush.getFalloffFactor(distance) * strength_mod;
	}
}
//
//SmoothTool::SmoothTool(Landscape * landscape_ptr, float strength, uint8_t levels)
//	:BasicTool(landscape_ptr), strength(strength), levels(levels) {}
//
//void SmoothTool::updatePositions(NewLandscapePiece * lp, Brush const& brush)
//{
//	std::vector<Vector3f> positions;
//
//	Vector2f xz_intersection_point{ last_intersection_point.x, last_intersection_point.z };
//
//	for (int i = 0; i < lp->length; ++i) {
//		for (int j = 0; j < lp->width; ++j) {
//			int begin_z = std::clamp<int>(i - levels, 0, lp->length - 1);
//			int end_z = std::clamp<int>(i + levels, 0, lp->length - 1);
//			
//			int begin_x = std::clamp<int>(j - levels, 0, lp->width - 1);
//			int end_x = std::clamp<int>(j + levels, 0, lp->width - 1);
//
//			uint16_t count = (end_x - begin_x + 1) * (end_z - begin_z + 1);
//			float height_summ = 0;
//
//			for (; begin_z <= end_z; ++begin_z) {
//				for (; begin_x <= end_x; ++begin_x) {
//					height_summ += lp->vertices_data[begin_z * lp->width + begin_x].first->position.y;
//				}
//			}
//			
//			auto position = lp->vertices_data[i * lp->width + j].first->position;
//
//			Vector2f xz_position{ position.x, position.z };
//
//			float distance = (xz_position - xz_intersection_point).lenght();
//
//			float height_increment = (position.y - (height_summ / count))  * strength * brush.getFalloffFactor(distance);
//			position.y -= height_increment;
//
//			positions.push_back(position);
//			/*std::cout << "height_increment: " << height_increment
//				<< " height_summ: " << height_summ
//				<< " count: " << count << std::endl;*/
//		}
//	}
//
//	for (size_t i = 0; i < lp->vertices_data.size(); ++i)
//		lp->vertices_data[i].first->position = positions[i];
//}