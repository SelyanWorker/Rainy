#pragma once

#include "Rainy/Rainy.h"
#include "Rainy/Core/Math.h"

//#include "../Landscape.h"
#include "LandscapePlus.h"
#include "BrushPlus.h"

using Rainy::Vector3f;

namespace ls {
	
	struct Ray;
	class BasicTool;
	class SculptTool;
	class SmoothTool;

	struct Ray 
	{
		Vector3f Start{ 0 };
		Vector3f Direction{ 0 };
		float Length{ 0 };
	};


	class BasicTool {

	public:
		//explicit BasicTool(Landscape * landscape_ptr);
		explicit BasicTool(LandscapePlus* landscape_ptr);

		virtual ~BasicTool() = default;

		//void setLandscape(Landscape * landscape_ptr);
		void SetLandscape(LandscapePlus* landscape_ptr);

		Vector3f GetLastIntersectionPoint() const { return m_lastInterPoint; }

		void UpdateLastIntersectionPoint(Vector3f point);

		virtual void ModifyLandscape(Brush* brush);

	protected:
		virtual void ModifyVertexPositions(SectionPart* lp, Brush* brush);

		virtual void ModifyVertexNormals(SectionPart* lp);

	protected:
		//Landscape * landscape_ptr;
		LandscapePlus* m_landscape;
		Vector3f m_lastInterPoint;
		Ray m_interRay;
	};

	class SculptTool : public BasicTool {
	public:
		//explicit SculptTool(Landscape * landscape_ptr, float strength);
		explicit SculptTool(LandscapePlus* landscape_ptr, float strength);

		~SculptTool() = default;

	protected:
		virtual void ModifyVertexPositions(SectionPart* lp, Brush* brush);

	public:
		float strength;
	};

	class SmoothTool : public BasicTool {
	public:
		float strength;
		uint8_t levels;

	public:
		explicit SmoothTool(LandscapePlus* landscape_ptr, float strength, uint8_t levels);

		~SmoothTool() = default;

	protected:
		virtual void ModifyVertexPositions(SectionPart* lp, Brush* brush);
	};
}