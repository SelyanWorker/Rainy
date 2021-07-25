#pragma once

#include "Rainy/Rainy.h"
#include "Rainy/Core/Math.h"

//#include "../Landscape.h"
#include "../../NewLandscape/NewLandscape.h"
#include "Brush.h"

using Rainy::Vector3f;


struct IntersectionRay;
class BasicTool;
class SculptTool;
class SmoothTool;

struct IntersectionRay {
	Vector3f start_position{ 0 };
	Vector3f direction{ 0 };

	float length{ 0 };
};


class BasicTool {
protected:
	//Landscape * landscape_ptr;
	NewLandscape * landscape_ptr;
	Vector3f last_intersection_point;
	IntersectionRay intersection_ray;

public:
	//explicit BasicTool(Landscape * landscape_ptr);
	explicit BasicTool(NewLandscape * landscape_ptr);

	virtual ~BasicTool() = default;

	//void setLandscape(Landscape * landscape_ptr);
	void setLandscape(NewLandscape * landscape_ptr);

	Vector3f getLastIntersectionPoint() const { return last_intersection_point; }

	void updateLastIntersectionPoint(IntersectionRay const & ray);

	virtual void updatePositions(NewLandscapePiece * lp, Brush const& brush);

	void updateNormals(NewLandscapePiece * lp);

	void update(Brush const & brush);

};

class SculptTool : public BasicTool {
public:
	float strength;
	
public:
	//explicit SculptTool(Landscape * landscape_ptr, float strength);
	explicit SculptTool(NewLandscape * landscape_ptr, float strength);
		
	~SculptTool() = default;

	void updatePositions(NewLandscapePiece * lp, Brush const& brush) override;
};

class SmoothTool : public BasicTool {
public:
	float strength;
	uint8_t levels;

public:
	explicit SmoothTool(NewLandscapePiece * landscape_ptr, float strength, uint8_t levels);

	~SmoothTool() = default;

	void updatePositions(NewLandscapePiece * lp, Brush const& brush) override;
};


