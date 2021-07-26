#pragma once

#include "math/Math.h"
#include "Terrainn.h"
#include "BrushT.h"

#include <algorithm>

using Rainy::Vector3f;

namespace te {

	enum class ToolTypes
	{
		NONE, SCULPT, SMOOTH, ADD_SECTION, DELETE_SECTION
	};

	class ToolCommand
	{
	public:
		virtual void Execute() = 0;

		virtual void Cancel() = 0;

	};

	class BasicToolCommand : public ToolCommand
	{
		using AreaParts = std::pair<TerrainArea*, std::vector<Rainy::AABB2Di>>;
	public:
		BasicToolCommand(Terrain* terrain, Vector3f interPoint, Brush brush)
		:	m_terrain(terrain),
			m_interPoint(interPoint),
			m_brush(brush)
		{
			TerrainArea* execut = m_terrain->GetArea({ m_interPoint.x, m_interPoint.z }, m_brush.size);
			TerrainArea* cancel = execut->GetCopy();
			Rainy::AABB2Di aabb{ { 0, 0 }, { int32_t(execut->Width), int32_t(execut->Height) } };

			m_areasToExecute.push_back({ execut, { aabb } });
			m_areasToCancel.push_back({ cancel, { aabb } });

			m_areasToCancel.reserve(2000);
		}

		virtual void Execute() override { }

		virtual void Cancel() override { }

		virtual void Merge(BasicToolCommand* command)
		{
			using Rainy::AABB2Di;

			auto trimmingProcess = [](AABB2Di const& temp, Vector2i offset, std::vector<AABB2Di>& parts)
			{
				if (Rainy::AABB2DZeroSpace(temp)) return;
				for (size_t j = 0; j < parts.size(); j++)
				{
					AABB2Di aabb = parts[j];
					aabb.Move(offset);

					AABB2Di overlay;
					if (Rainy::AABB2DOverlay(temp, aabb, overlay))
					{
						auto aabbParts = Rainy::AABB2DExcludeOverlay(aabb, overlay);
						if (!aabbParts.empty())
						{
							AABB2Di first = aabbParts.front();
							first.Move(-offset);
							parts[j] = first;

							for (auto q = aabbParts.begin() + 1; q < aabbParts.end(); q++)
							{
								q->Move(-offset);
								parts.push_back((*q));
							}
						}
					}
				}
			};

			auto& mcExecutePair = command->m_areasToExecute[0];
			auto& mcCancelPair = command->m_areasToCancel[0];
			TerrainArea* mcTerrainArea = mcCancelPair.first;

			auto& mcCancelAABBs = mcCancelPair.second;

			Vector2i const mcFi = mcTerrainArea->FirstIndex;
			AABB2Di mcAABB{ mcFi, Vector2i{ int32_t(mcTerrainArea->Width), int32_t(mcTerrainArea->Height) } + mcFi };

			bool addCancelPair = true;

			static std::initializer_list<AABB2Di> nullAABB{ AABB2Di{ Vector2i{ 0 }, Vector2i{ 0 } } };

			for (size_t i = 0; i < m_areasToCancel.size(); i++)
			{
				auto& executePair = m_areasToExecute[i];
				auto& cancelPair = m_areasToCancel[m_areasToCancel.size() - 1 - i];

				Vector2i const fi = executePair.first->FirstIndex;

				auto& executeAABBs = executePair.second;

				if (mcFi == fi)
				{
					addCancelPair = false;
					executeAABBs = nullAABB;
				}
				else
				{
					if (addCancelPair)
					{
						AABB2Di cancelAreaAABB{ fi, Vector2i{ int32_t(cancelPair.first->Width), int32_t(cancelPair.first->Height) } +fi };
						trimmingProcess(cancelAreaAABB, mcFi, mcCancelAABBs);
					}

					trimmingProcess(mcAABB, fi, executeAABBs);
				}

			}

			if(!addCancelPair) m_areasToCancel.push_back({ mcTerrainArea, nullAABB });
			else m_areasToCancel.push_back(mcCancelPair);

			m_areasToExecute.push_back(mcExecutePair);
		}

	protected:
		virtual void ModifyPositions(TerrainArea* area, Vector3f interPoint) { }

		//virtual void RecalcNormals(TerrainArea* area);

	protected:
		Terrain* m_terrain;
		Vector3f m_interPoint;
		Brush m_brush;
		std::vector<AreaParts> m_areasToExecute;
		std::vector<AreaParts> m_areasToCancel;
	};

	class ScupltToolCommand : public BasicToolCommand
	{

	public:
		ScupltToolCommand(Terrain* terrain, Vector3f interPoint, Brush brush, float strength)
		:	BasicToolCommand(terrain, interPoint, brush),
			m_strength(strength),
			m_firstExecute(true)
		{}

		void Execute() override
		{
			if (m_firstExecute)
			{
				m_firstExecute = false;
				ModifyPositions(m_areasToExecute[0].first, m_interPoint);
				//RecalcNormals(m_areasToExecute[0].first);
				m_terrain->ApplyArea(m_areasToExecute[0].first);
			}
			else
			{
				for (auto i = m_areasToExecute.begin(); i < m_areasToExecute.end(); i++)
					m_terrain->ApplyAreaParts(i->first, i->second);
			}
		}

		void Cancel() override
		{
			for (auto i = m_areasToCancel.rbegin(); i < m_areasToCancel.rend(); i++)
				m_terrain->ApplyAreaParts(i->first, i->second);
		}

	protected:
		void ModifyPositions(TerrainArea* area, Vector3f interPoint) override;

	private:
		float m_strength;
		bool m_firstExecute;
	};

	class SmoothToolCommand : public BasicToolCommand
	{

	public:
		SmoothToolCommand(Terrain* terrain, Vector3f interPoint, Brush brush, float strength, uint32_t levels)
		:	BasicToolCommand(terrain, interPoint, brush),
			m_strength(strength),
			m_levels(levels),
			m_firstExecute(true)
		{}

		void Execute() override
		{
			if (m_firstExecute)
			{
				m_firstExecute = false;
				ModifyPositions(m_areasToExecute[0].first, m_interPoint);
			//	RecalcNormals(m_areasToExecute[0].first);
				m_terrain->ApplyArea(m_areasToExecute[0].first);
			}
			else
			{
				for (auto area : m_areasToExecute) m_terrain->ApplyAreaParts(area.first, area.second);
			}
		}

		void Cancel() override
		{
			for (auto i = m_areasToCancel.rbegin(); i < m_areasToCancel.rend(); i++)
				m_terrain->ApplyAreaParts((*i).first, (*i).second);
		}

	protected:
		void ModifyPositions(TerrainArea* area, Vector3f interPoint) override;

	private:
		float m_strength;
		uint32_t m_levels;
		bool m_firstExecute;
	};

	class ToolManager
	{
		static constexpr float MIN_STRENGTH_MOD = 0.25f;

	public:
		ToolManager(Terrain* terrain)
		:	m_terrain(terrain),
			m_merge(false)
		{}

		void BeginEdit()
		{

		}

		void EndEdit()
		{
			m_merge = false;
			//m_editBegin = false;
		}

		void Redo()
		{
			if (m_redoCommands.empty()) return;
			BasicToolCommand* command = m_redoCommands.back();
			command->Execute();

			m_redoCommands.pop_back();
			m_undoCommands.push_back(command);
		}

		void Undo()
		{
			if (m_undoCommands.empty()) return;
			BasicToolCommand* command = m_undoCommands.back();
			command->Cancel();

			m_undoCommands.pop_back();
			m_redoCommands.push_back(command);
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

			BasicToolCommand* sculptCommand =
				new ScupltToolCommand(m_terrain, interPoint, brush, strength * strength_mod);

			static uint32_t mergeCount = 0;
			sculptCommand->Execute();
			if (!m_merge)
			{
				RN_APP_INFO("Merge count: {0}", mergeCount);
				m_undoCommands.push_back(sculptCommand);
				m_redoCommands.clear();
				m_merge = true;
				mergeCount = 0;
			}
			else
			{
				m_undoCommands.back()->Merge(sculptCommand);
				mergeCount++;
			}
		}

		void ApplySmooth(Brush brush, Vector3f interPoint, float strength, uint32_t levels)
		{
			SmoothToolCommand* smoothCommand =
				new SmoothToolCommand(m_terrain, interPoint, brush, strength, levels);

			static uint32_t mergeCount = 0;
			smoothCommand->Execute();
			if (!m_merge)
			{
				RN_APP_INFO("Merge count: {0}", mergeCount);
				m_undoCommands.push_back(smoothCommand);
				m_redoCommands.clear();
				m_merge = true;
				mergeCount = 0;
			}
			else
			{
				m_undoCommands.back()->Merge(smoothCommand);
				mergeCount++;
			}
		}

	private:
		bool m_merge;
		std::vector<BasicToolCommand*> m_undoCommands;
		std::vector<BasicToolCommand*> m_redoCommands;
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
