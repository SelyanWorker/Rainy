#pragma once

#pragma once

#include "Rainy/Rainy.h"
#include "Rainy/Core/Math.h"

//#include "../Terrain.h"
#include "Terrainn.h"
#include "BrushT.h"

using Rainy::Vector3f;

namespace te {

	enum class ToolTypes
	{
		NONE, SCULPT, SMOOTH
	};

	class ToolManagerCommand
	{
	public:
		virtual void Execute() = 0;

		virtual void Cancel() = 0;

	};

	class BasicToolCommand : public ToolManagerCommand
	{
	public:
		BasicToolCommand(Terrain* terrain, Vector3f interPoint, Brush brush)
			: m_terrain(terrain),
			m_interPoint(interPoint),
			m_brush(brush)
		{
			m_workArea = m_terrain->GetArea({ m_interPoint.x, m_interPoint.z }, m_brush.size);
		}

		virtual void Execute() override { }

		virtual void Cancel() override { }

		virtual void Test_ReversibilityWorkArea()
		{
			TerrainArea copyOfArea = *m_workArea;
			Execute();
			Cancel();
			bool equal = false;
			for (uint32_t y = 0; y < m_workArea->Height; y++)
			{
				for (uint32_t x = 0; x < m_workArea->Width; x++)
				{
					uint32_t i = y * m_workArea->Width + x;
					equal = m_workArea->Positions[i] == copyOfArea.Positions[i];
					equal = equal && m_workArea->Normals[i] == copyOfArea.Normals[i];
				}
			}
			RN_APP_INFO("Test_ReversibilityWorkArea result: {0}", equal);
		}

	protected:
		virtual void ModifyPositions(TerrainArea* area, Vector3f interPoint) {}

		virtual void RecalcNormals(TerrainArea* area);

	protected:
		Terrain* m_terrain;
		Vector3f m_interPoint;
		Brush m_brush;
		TerrainArea* m_workArea;
	};

	class ScupltToolCommand : public BasicToolCommand
	{
	public:
		ScupltToolCommand(Terrain* terrain, Vector3f interPoint, Brush brush, float strength)
			: BasicToolCommand(terrain, interPoint, brush),
			m_strength(strength)
		{}

		void Execute() override
		{
			ModifyPositions(m_workArea, m_interPoint);
			RecalcNormals(m_workArea);
			m_terrain->ApplyChanges(m_workArea);
		}

		void Cancel() override
		{
			m_strength = -m_strength;
			Execute();
		}

	protected:
		void ModifyPositions(TerrainArea* area, Vector3f interPoint) override;

	private:
		float m_strength;
	};

	class SmoothToolCommand : public BasicToolCommand
	{
	public:
		SmoothToolCommand(Terrain* terrain, Vector3f interPoint, Brush brush, float strength, uint32_t levels)
			: BasicToolCommand(terrain, interPoint, brush),
			m_strength(strength),
			m_levels(levels)
		{}

		void Execute() override
		{
			ModifyPositions(m_workArea, m_interPoint);
			RecalcNormals(m_workArea);
			m_terrain->ApplyChanges(m_workArea);
		}

		void Cancel() override
		{
			m_strength = -m_strength;
			Execute();
		}

	protected:
		void ModifyPositions(TerrainArea* area, Vector3f interPoint) override;

	private:
		float m_strength;
		uint32_t m_levels;
	};

	class ToolManager
	{
		static constexpr float MIN_STRENGTH_MOD = 0.25f;

	public:
		ToolManager(Terrain* terrain)
			: m_terrain(terrain)
		{
			
		}

		void ApplySculpt(Brush brush, Vector3f interPoint, float strength)
		{
			static std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
			static auto lastInterPoint = interPoint;
			static float strength_mod = 0;
			if (lastInterPoint == interPoint) {
				strength_mod = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count()) / 2000.f;
			}
			else {
				strength_mod = MIN_STRENGTH_MOD;
				time = std::chrono::steady_clock::now();
				lastInterPoint = interPoint;
			}

			strength_mod = std::clamp<float>(strength_mod, MIN_STRENGTH_MOD, 1.f);

			ScupltToolCommand* sculptCommand = 
				new ScupltToolCommand(m_terrain, interPoint, brush, strength * strength_mod);

			sculptCommand->Test_ReversibilityWorkArea();

			sculptCommand->Execute();
			m_doCommands.push_back(sculptCommand);
		}

		void ApplySmooth(Brush brush, Vector3f interPoint, float strength, uint32_t levels)
		{
			SmoothToolCommand* smoothCommand =
				new SmoothToolCommand(m_terrain, interPoint, brush, strength, levels);

			smoothCommand->Test_ReversibilityWorkArea();

			smoothCommand->Execute();
			m_doCommands.push_back(smoothCommand);
		}

		void Redo()
		{

		}

		void Undo()
		{
			if (m_doCommands.size() == 0) return;
			ToolManagerCommand* command = m_doCommands.back();
			m_doCommands.pop_back();
			command->Cancel();
		}

	private:
		std::vector<ToolManagerCommand*> m_doCommands;
		std::vector<ToolManagerCommand*> m_unDoCommands;
		//std::vector<ToolManagerCommand*>::iterator m_currComm;
		Vector3f m_interPoint;
		Terrain* m_terrain;
	};

	/*class Tool
	{
	public:
		void ModifyTerrain(Terrain* terrain, Vector3f interPoint);

		static ToolTypes GetType() { return ToolTypes::NONE; }

	protected:
		virtual void ModifyPositions(TerrainArea* area, Vector3f interPoint) = 0;

		virtual void ModifyNormals(TerrainArea* area);

	};

	class SculptTool : public Tool
	{
	public:
		SculptTool(Brush brush, float strength);

		~SculptTool() = default;

		static ToolTypes GetType() { return ToolTypes::SCULPT; }

	protected:
		void ModifyPositions(TerrainArea* area, Vector3f interPoint) override;

	private:
		Brush m_brush;
		float m_strength;
	};

	class SmoothTool : public Tool
	{
	public:
		SmoothTool(Brush brush, float strength, uint8_t levels);

		~SmoothTool() = default;

		static ToolTypes GetType() { return ToolTypes::SMOOTH; }

	protected:
		void ModifyPositions(TerrainArea* area, Vector3f interPoint) override;
	
	private:
		Brush m_brush;
		float m_strength;
		uint8_t m_levels;
	};*/
}