#pragma once

#include "Rainy/Rainy.h"
#include "Rainy/Core/Math.h"

//#include "../Terrain.h"
#include "Terrainn.h"
#include "BrushT.h"

using Rainy::Vector3f;

namespace te {
	
	class BasicTool 
	{
	public:
		BasicTool();

		virtual ~BasicTool() = default;

		void SetBrush(Brush* brush) { m_brush = brush; }

		Brush* GetBrush() const { return m_brush; }

		void ModifyTerrain(Terrain* terrain, Vector3f interPoint);

	protected:
		virtual void ModifyPositions(TerrainArea* area) {}

		virtual void ModifyNormals(TerrainArea* area);

	protected:
		Brush* m_brush;
		Vector3f m_interPoint;
	};

	class SculptTool : public BasicTool 
	{
	public:
		SculptTool(float strength);

		~SculptTool() = default;

		void SetStrength(float strength) { m_strength = strength; }

		float GetStrength() const { return m_strength; }

	protected:
		void ModifyPositions(TerrainArea* area) override;

	private:
		float m_strength;
	};

	class SmoothTool : public BasicTool 
	{
	public:
		SmoothTool(float strength, uint8_t levels);

		~SmoothTool() = default;

		void SetStrength(float strength) { m_strength = strength; }

		float GetStrength() const { return m_strength; }

		void SetLevel(uint8_t levels) { m_levels = levels; }

		uint8_t GetLevels() const { return m_levels; }

	protected:
		void ModifyPositions(TerrainArea* area) override;

	private:
		float m_strength;
		uint8_t m_levels;
	};
}