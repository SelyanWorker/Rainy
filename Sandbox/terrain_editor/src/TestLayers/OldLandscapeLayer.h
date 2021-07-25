#pragma once

#include "Rainy/Rainy.h"
#include "Rainy/Core/EntryPoint.h"
#include "Rainy/Core/Time.h"
#include "Rainy/Core/Input.h"
#include "Rainy/Core/KeyCodes.h"
#include "Rainy/Core/Time.h"

#include "Rainy/Utils/Image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Rainy/Render/Buffer.h"
#include "Rainy/Render/VertexArray.h"
#include "Rainy/Render/FrameBuffer.h"
#include "Rainy/Render/Renderer.h"
#include "Rainy/Render/Texture2D.h"
#include "Rainy/Render/ShaderLibrary.h"
#include "Rainy/Render/Cameras/OrthographicCamera.h"
#include "Rainy/Render/Cameras/PerspectiveCamera.h"

#include "../LandscapePlus/LandscapePlus.h"
#include "../LandscapePlus/ToolPlus.h"
#include "../LandscapePlus/BrushPlus.h"

#include "Rainy/3D/Light/Light.h"

using namespace ls;

class LandscapePlusLayer : public Rainy::Layer
{
	enum class ToolType
	{
		SCULPT, SMOOTH, ADD_SECTION, DELETE_SECTION
	};

public:
	LandscapePlusLayer()
	{
		m_frameBuffer = Rainy::FrameBuffer::Create(m_contentAreaWidth, m_contentAreatHeight);
		m_shaderProgram = Rainy::ShaderLibrary::CreateShader("res/shaders/Landscape.glsl");
		m_pointsShader = Rainy::ShaderLibrary::CreateShader("res/shaders/Points.glsl");
		m_baseShader = Rainy::ShaderLibrary::CreateShader("res/shaders/Base.glsl");

		float aspectRation = float(m_contentAreaWidth) / m_contentAreatHeight;
		float widthHalf = float(m_contentAreaWidth) / 2;
		float heightHalf = float(m_contentAreatHeight) / 2;
		//RN_CORE_INFO("AspectRatio: {0}", aspectRation);
		m_perspCamera = new Rainy::PerspectiveCamera(aspectRation, 60.f, 0.1f, 100.f,
			{ 0, 5, -10 }, { -30, 0, 0 }, 15.f, 0.1f);

		//Rainy::EnablePolygonMode();

		m_landscape = new Landscape(LandscapeInitSize::_2, SectionSize::_127);
		m_landscape->SetScale(0.01f);
		
		//LandscapeTester::GetSurfacePoint_Test(m_landscape);

		m_intersectionPoints.reserve(100);

		m_interPointsBuffer = VertexBuffer::Create(m_intersectionPoints.capacity() * sizeof(Vector3f),
			m_intersectionPoints.data());
		auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false) };
		m_interPointsBuffer->SetBufferLayout({ elements, m_intersectionPoints.capacity() });
		m_interPointsVertexArray = VertexArray::Create();
		m_interPointsVertexArray->SetVertexBuffers({ m_interPointsBuffer });

		m_brush = new ls::Brush(10.f, 0.5f);
		m_sculptTool = new SculptTool(m_landscape, 10.1f);
		m_smoothTool = new SmoothTool(m_landscape, 0.1f, 10);
		m_toolType = ToolType::SCULPT;
		m_sunStrength = 0.5f;

		m_light = new Rainy::Light();
		m_light->SetPosition({ 0, 10, 0 });

		// image test
		/*struct Pixel { uint8_t Red; uint8_t Green; uint8_t Blue; };
		Pixel* pixels0 = new Pixel[200 * 200];
		for (uint32_t i = 0; i < 200 * 200; i++) pixels0[i] = { 255, 0, 0 };
		Rainy::Image* image0 = Rainy::Image::Create((uint8_t*)pixels0, 3, 200, 200);
		bool result = image0->SaveImage("SaveTest.png", Rainy::ImageTypes::RN_PNG);*/
	}

	~LandscapePlusLayer()
	{}

	void OnEvent(Rainy::Event& e) override
	{}

	void OnUpdate() override
	{
		using Rainy::Input;

		if (Input::IsKeyPressed(Rainy::RN_KEY_UP))
			m_sunStrength += Rainy::Application::Get()->GetFrameTime();
		if (Input::IsKeyPressed(Rainy::RN_KEY_DOWN))
			m_sunStrength -= Rainy::Application::Get()->GetFrameTime();
		
		if (Input::IsKeyPressed(Rainy::RN_KEY_I))
		{
			Rainy::Image* image = ls::CreateHeightMapImage(m_landscape);
			image->SaveImage("HeightMap.png", Rainy::ImageTypes::RN_PNG);
			delete image;
		}

		if (m_focused && Input::IsKeyPressed(Rainy::RN_KEY_LEFT_ALT))
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
			Rainy::Application::Get()->DisableCursor();
			m_perspCamera->UnFreeze();
		}
		else
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
			Rainy::Application::Get()->EnableCursor();
			m_perspCamera->Freeze();
		}

		m_perspCamera->OnUpdate();

		// old tool code
		Vector3f rayDir = ViewportSpaceToWorldSpace(
			m_windowCursorPos, m_perspCamera->GetProjectionMatrix(), m_perspCamera->GetViewMatrix(),
			m_contentAreaWidth, m_contentAreatHeight);
		rayDir.normalize();

		constexpr float RAY_DISTANCE = 10000.f;
		/*	current_tool->updateLastIntersectionPoint({ m_perspCamera->GetPosition(),
				rayDir, RAY_DISTANCE });*/
				//static Vector3f interPoint = current_tool->getLastIntersectionPoint();
				/*if (interPoint != current_tool->getLastIntersectionPoint())
					interPoint = current_tool->getLastIntersectionPoint();*/

					//static Vector3f interPoint;

	
		//RN_APP_TRACE("RayDir: dir {0}, interPoint {1}", rayDir.str(), interPoint.str());
		//static bool keyRelease = Input::IsKeyReleased(Rainy::RN_KEY_C);
		if (Input::IsKeyPressed(Rainy::RN_KEY_1))
			m_toolType = ToolType::SCULPT;
		if (Input::IsKeyPressed(Rainy::RN_KEY_2))
			m_toolType = ToolType::SMOOTH;
		if (Input::IsKeyPressed(Rainy::RN_KEY_3))
			m_toolType = ToolType::ADD_SECTION;
		if (Input::IsKeyPressed(Rainy::RN_KEY_4))
			m_toolType = ToolType::DELETE_SECTION;

		// ћожет быть рассчитывать точку Intersection point по нажатию мыши? 
		bool intersaction =
			m_landscape->RayIntersection(m_interPoint, rayDir, m_perspCamera->GetPosition(), RAY_DISTANCE);

		if (Input::IsMouseKeyPressed(Rainy::RN_MOUSE::BUTTON_1))
		{
			// passed time in milliseconds
			using Rainy::TimeStep;

			static constexpr float DELAY = 40;
			static float prevTime = TimeStep::GetTime().GetMilli() - DELAY;
			TimeStep currentTime = TimeStep::GetTime();
			//if (m_perspCamera->IsFreezed() && (currentTime.GetMilli() - prevTime) >= DELAY)
			if(m_perspCamera->IsFreezed())
			{
				//RN_APP_TRACE("RayDir {0}", rayDir.str());
				
				/*RN_APP_TRACE("Intersaction: result {0}, point {1}",
					intersaction, interPoint.str());*/
			
				//RN_APP_TRACE("RayDir : {0} CamPos : {1}", rayDir.str(), m_perspCamera->GetPosition().str());

				if (m_toolType == ToolType::ADD_SECTION)
					m_landscape->AddSection(m_interPoint);
				else if(m_toolType == ToolType::DELETE_SECTION)
					m_landscape->RemoveSection(m_interPoint);

				if (intersaction)
				{
					switch (m_toolType)
					{
					case ToolType::SCULPT: 
						m_sculptTool->UpdateLastIntersectionPoint(m_interPoint);
						m_sculptTool->ModifyLandscape(m_brush);
						break;
					case ToolType::SMOOTH:
						m_smoothTool->UpdateLastIntersectionPoint(m_interPoint);
						m_smoothTool->ModifyLandscape(m_brush);
						break;
					}

					Vector3f scaledInterPoint = m_interPoint * m_landscape->GetScale();
					m_intersectionPoints.push_back(scaledInterPoint);

					static uint32_t prevCapacity = m_intersectionPoints.capacity();
					uint64_t pointCount = m_intersectionPoints.size();

					auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false) };
					m_interPointsBuffer->SetBufferLayout({ elements, pointCount });

					if (prevCapacity < pointCount)
					{
						m_intersectionPoints.reserve(pointCount * 2);
						prevCapacity = m_intersectionPoints.capacity();
						m_interPointsBuffer->Reallocate(m_intersectionPoints.capacity() * sizeof(Vector3f), nullptr);
						m_interPointsBuffer->SubData(0, m_intersectionPoints.size() * sizeof(Vector3f),
							m_intersectionPoints.data());
					}
					else
					{
						m_interPointsBuffer->SubData((m_intersectionPoints.size() - 1) * sizeof(Vector3f),
							sizeof(Vector3f), &m_intersectionPoints.back());
					}
				}
			}
			prevTime = currentTime.GetMilli();
		}
		//
		Render();

		static float timeSum = 0;
		float frameTime = Rainy::Application::Get()->GetFrameTime();
		timeSum += frameTime;

		if (timeSum >= 1)
		{
			RN_APP_TRACE("FPS: {0}", 1.f / frameTime);
			timeSum = 0;
		}
	}

	void Render()
	{
		m_frameBuffer->Bind();
		Rainy::Clear();

		// draw light box
		m_baseShader->Bind();
		m_baseShader->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_baseShader->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		m_baseShader->SetUniform("mainColor", Rainy::Vector4f( 1.f, 0, 0, 1.f ));
		m_light->DrawLightBox(m_baseShader);
		//

		m_shaderProgram->Bind();
		m_shaderProgram->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_shaderProgram->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		m_shaderProgram->SetUniform("modelMatrix", m_landscape->GetModelMatrix());
		m_shaderProgram->SetUniform("mainColor", Rainy::Vector4f(0.15f, 0.15f, 0.15f, 1));
		/*m_shaderProgram->SetUniform("intensity", m_sunStrength);
		m_shaderProgram->SetUniform("lightPosition", Rainy::Vector3f(0, 10, 0));*/
		m_shaderProgram->SetUniform("brushSize", m_brush->size * m_landscape->GetScale());
		m_shaderProgram->SetUniform("brushColor", Rainy::Vector4f(0.1f, 0.3f, 0.1f, 0.5f));
		m_shaderProgram->SetUniform("brushCenter", m_interPoint * m_landscape->GetScale());

		m_light->ToShader(m_shaderProgram);
		
		
		//Rainy::EnableCullFace();
		m_landscape->Draw();
		//Rainy::DisableCullFace();

		float prevPointSize = Rainy::GetPointSize();
		Rainy::SetPointSize(3.f);
		m_pointsShader->Bind();
		m_pointsShader->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_pointsShader->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		m_pointsShader->SetUniform("pointColor", Rainy::Vector4f(1.f, 0, 0, 1));

		//Rainy::DrawArray(Rainy::RN_POINTS, m_interPointsArrayBuffer, m_intersectionPoints.size());
		Rainy::DrawVertexArray(Rainy::RN_POINTS, m_interPointsVertexArray);
		Rainy::SetPointSize(prevPointSize);

		m_frameBuffer->UnBind();
	}

	void OnImGuiRender() override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		static uint16_t titleHeight = ImGui::GetFrameHeight();

		ImGui::Begin("LandscapePlusLayer");

		float windowWidth = ImGui::GetWindowWidth();
		float windowHeight = ImGui::GetWindowHeight();

		ImVec2 currentWinPos = ImGui::GetCurrentWindow()->Pos;

		auto cursorPosition = Rainy::Input::GetCursorPosition();
		auto windowPosition = Rainy::Application::Get()->GetWindow()->GetWindowsPosition();

		m_windowCursorPos = { cursorPosition.first - windowPosition.first - currentWinPos.x,
			cursorPosition.second - windowPosition.second - currentWinPos.y - titleHeight };
	
	/*	RN_APP_TRACE("WinPos : x{0}, y{1} m_windowCursorPos : x{2}, y{3}",
			currentWinPos.x, currentWinPos.y, cursorPosition.first, cursorPosition.second);
*/
		if (windowWidth != m_contentAreaWidth || (windowHeight - titleHeight) != m_contentAreatHeight)
		{
			m_contentAreaWidth = windowWidth;
			m_contentAreatHeight = windowHeight - titleHeight;

			m_frameBuffer->SetSize(m_contentAreaWidth, m_contentAreatHeight);
			m_perspCamera->RecreateProjectionMatrix(float(m_contentAreaWidth) / m_contentAreatHeight, 60.f, 0.1f, 100.f);

			Render();
		}

		ImGui::GetWindowDrawList()->AddImage(
			(void *)m_frameBuffer->GetNative(), ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + windowWidth, ImGui::GetCursorScreenPos().y + windowHeight),
			ImVec2(0, 1), ImVec2(1, 0)
		);
		// set camera update flag
		m_focused = ImGui::IsWindowFocused();

		ImGui::End();

		ImGui::PopStyleVar();
	}

private:
	uint32_t m_contentAreaWidth = 1920;
	uint32_t m_contentAreatHeight = 1080;

	Vector2f m_windowCursorPos;
	bool m_focused;

	Rainy::FrameBuffer* m_frameBuffer;
	Rainy::Shader* m_shaderProgram;
	Rainy::Shader* m_baseShader;

	Rainy::Shader* m_pointsShader;
	std::vector<Vector3f> m_intersectionPoints;
	VertexBuffer* m_interPointsBuffer;
	VertexArray* m_interPointsVertexArray;

	Rainy::Light* m_light;
	Landscape* m_landscape;
	SculptTool* m_sculptTool;
	SmoothTool* m_smoothTool;
	ls::Brush* m_brush;
	ToolType m_toolType;
	float m_sunStrength;
	Vector3f m_interPoint;

	Rainy::PerspectiveCamera* m_perspCamera;
};
